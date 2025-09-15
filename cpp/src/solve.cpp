#include "dp.h"
#include "omp.h"
#include "prob.h"
#include "rmab.h"
#include "utils.h"

#include <limits>
#include <numeric>

using namespace std;

double calculate_reward(const BitArray &state, const vector<int> &action,
                        const vector<pair<double, double>> &rewards) {
  double reward = 0;
  for (int i = 0; i < action.size(); ++i) {
    reward += (state[i] - action[i]) * rewards[i].first +
              action[i] * rewards[i].second;
  }
  return reward;
}

double RMAB::solve(int n_arms) {
  int n_alpha = n_arms * alpha;
  if (n_alpha / alpha != n_arms) {
    cerr << "Warning: Integer floor precision error detected in n_alpha "
            "calculation\n";
  }
  vector<int> initial_state(n_states);
  for (int i = 0; i < n_states; ++i) {
    int n = n_arms * initial_prop[i];
    if (n / initial_prop[i] != n_arms) {
      cerr << "Warning: Integer floor precision error detected in initial "
              "state calculation\n";
    }
    initial_state[i] = n;
  }
  if (accumulate(initial_state.begin(), initial_state.end(), 0) != n_arms) {
    cerr << "Warning: Initial state does not sum to number of arms\n";
  }

  MultiDist md(n_arms, n_states);
  DPLayer prev(n_arms, n_states), curr(n_arms, n_states);
  DPStates dps(n_arms, n_states);
  for (int t = n_steps - 1; t >= 0; --t) {
#pragma omp parallel for schedule(dynamic)
    for (int idx = 0; idx < dps.size(); ++idx) {
      const auto &curr_state = dps[idx];
      StateActionIterator ait(curr_state, n_states, n_alpha);
      double mx_reward = numeric_limits<double>::lowest();
      BitArray best_action;
      while (ait.next()) {
        double reward = 0;
        if(t < n_steps - 1) {
          auto dist = md.fullTransitionDistribution(
              curr_state, ait.current(), transition_probabilities[t].first,
              transition_probabilities[t].second);
          for (int i = 0; i < dps.size(); ++i) {
            const auto &nxt_state = dps[i];
            reward += dist[nxt_state] * prev[nxt_state].expectation;
          }
        }
        reward += calculate_reward(curr_state, ait.current(), rewards[t]);
        if(reward > mx_reward) {
          mx_reward = reward;
          best_action = BitArray(ait.current());
        }
        mx_reward = max(mx_reward, reward);
      }
      curr[curr_state] = {mx_reward, best_action};
    }
    swap(curr, prev);
  }

  for(int i = 0; i < dps.size(); ++i) {
    const auto& state = dps[i];
    if(BitArray(initial_state) == state) {
      const auto& res = prev[state];
      cout << "Best Action:\n";
      for(int i : res.best_action.to_vector()) {
        cout << i << ' ';
      }
      cout << '\n';
      return res.expectation / n_arms;
    }
  }
  return -1;
}