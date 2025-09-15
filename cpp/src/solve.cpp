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

DPValue RMAB::solve(int n_arms) {
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

  auto eval_state = [&](const DPState & state, int t) {
    StateActionIterator ait(state, n_states, n_alpha);
    double mx_reward = numeric_limits<double>::lowest();
    BitArray best_action;
    while (ait.next()) {
      double reward = 0;
      if(t < n_steps - 1) {
        auto dist = md.fullTransitionDistribution(
          state, ait.current(), transition_probabilities[t].first,
            transition_probabilities[t].second);
        for (int i = 0; i < dps.size(); ++i) {
          const auto &nxt_state = dps[i];
          reward += dist[nxt_state] * prev[nxt_state].expectation;
        }
      }
      reward += calculate_reward(state, ait.current(), rewards[t]);
      if(reward > mx_reward) {
        mx_reward = reward;
        best_action = BitArray(ait.current());
      }
    }
    return DPValue(mx_reward, best_action);
  };
  for (int t = n_steps - 1; t > 0; --t) {
#pragma omp parallel for schedule(dynamic)
    for (int idx = 0; idx < dps.size(); ++idx) {
      const auto &curr_state = dps[idx];
      curr[curr_state] = eval_state(curr_state, t);
    }
    swap(curr, prev);
  }
  return eval_state(DPState(BitArray(initial_state), n_arms, n_states), 0);
}