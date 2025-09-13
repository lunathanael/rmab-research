#include "dp.h"
#include "prob.h"
#include "rmab.h"
#include "utils.h"

#include <limits>

using namespace std;

double calculate_reward(const vector<int> &state, const vector<int> &action,
                        const std::vector<std::pair<double, double>> &rewards) {
  double reward = 0;
  for (int i = 0; i < state.size(); ++i) {
    reward += (state[i] - action[i]) * rewards[i].first +
              action[i] * rewards[i].second;
  }
  return reward;
}

void RMAB::solve() {
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
          reward += dist[sit2.current()] * prev[sit2];
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
      cout << prev[sit] / n_arms << endl;
      break;
    }
  }
}