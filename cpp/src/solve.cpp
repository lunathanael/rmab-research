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
      while (ait.next()) {
        auto dist = md.fullTransitionDistribution(
            curr_state, ait.current(), transition_probabilities[t].first,
            transition_probabilities[t].second);
        double reward = 0;
        for (int idx = 0; idx < dps.size(); ++idx) {
          const auto &nxt_state = dps[idx];
          reward += dist[nxt_state] * prev[nxt_state];
        }
        reward += calculate_reward(curr_state, ait.current(), rewards[t]);
        mx_reward = max(mx_reward, reward);
      }
      curr[curr_state] = mx_reward;
    }
    swap(curr, prev);
  }

  for (int i = 0; i < dps.size(); ++i) {
    const auto &state = dps[i];
    const auto statevec = state.to_vector();
    if (equal(initial_state.begin(), initial_state.end(), statevec.begin())) {
      return prev[state] / n_arms;
    }
  }
  return -1;
}