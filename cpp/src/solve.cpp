#include "dp.h"
#include "prob.h"
#include "rmab.h"
#include "utils.h"

#include <limits>
#include <numeric>

using namespace std;

double calculate_reward(const vector<int> &state, const vector<int> &action,
                        const vector<pair<double, double>> &rewards) {
  double reward = 0;
  for (int i = 0; i < state.size(); ++i) {
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

  DPLayer prev(n_arms, n_states), curr(n_arms, n_states);
  DPStateIterator sit(n_arms, n_states), sit2(n_arms, n_states);
  for (int t = n_steps - 1; t >= 0; --t) {
    sit.init();
    do {
      StateActionIterator ait(sit, n_alpha);
      double mx_reward = numeric_limits<double>::lowest();
      while (ait.next()) {
        auto dist = fullTransitionDistribution(
            sit.current(), ait.current(), transition_probabilities[t].first,
            transition_probabilities[t].second);
        double reward = 0;
        sit2.init();
        do {
          reward += dist[sit2.current_hash()] * prev[sit2];
        } while (sit2.next());
        reward += calculate_reward(sit.current(), ait.current(), rewards[t]);
        mx_reward = max(mx_reward, reward);
      }
      curr[sit] = mx_reward;
    } while (sit.next());
    swap(curr, prev);
  }

  sit.init();
  for (int i = 0; i < sit.size(); i = sit.next()) {
    if (equal(sit.current().begin(), sit.current().end(),
              initial_state.begin())) {
      return prev[sit] / n_arms;
    }
  }
  return -1;
}