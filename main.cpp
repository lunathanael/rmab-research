#include "utils.h"
#include <bits/stdc++.h>

using namespace std;

template <
    int H, int N, int S, double alpha, array<int, S> initial_state,
    array<array<array<double, 2>, S>, H> rewards,
    array<array<array<array<double, S>, S>, 2>, H> transition_probabilities>
class RMAB {
public:
  static constexpr const int alpha_n = alpha * N;
  static_assert(alpha_n / alpha == N, "alpha * N must be an integer");
  static constexpr const auto states = compile_time::generate_states<N, S>();
  std::array<std::vector<std::array<int, S>>, states.size()> actions;
  unordered_map<array<int, S>, int, hash_array<S>> state_to_idx;
  array<array<vector<double>, states.size()>, H> r;
  array<array<array<array<double, states.size()>, states.size()>, states.size()>, H> p;

  static constexpr auto calculate_rewards(int t, array<int, S> s,
                                          array<int, S> a) {
    double res = 0;
    for (int i = 0; i < S; ++i) {
      res += rewards[t][i][1] * a[i];
      res += rewards[t][i][0] * (s[i] - a[i]);
    }
    return res;
  }

  void precompute_rewards() {
    for (int t = 0; t < H; ++t) {
      for (int i = 0; i < states.size(); ++i) {
        r[t][i].resize(actions[i].size());
        for (int j = 0; j < actions[i].size(); ++j) {
          r[t][i][j] = calculate_rewards(t, states[i], actions[i][j]);
        }
      }
    }
  }

  void precompute_transition_probabilities() {
    for (int t = 0; t < H; ++t) {
      for (int i = 0; i < states.size(); ++i) {
        for (int j = 0; j < actions[i].size(); ++j) {
          auto op_a = subtract_array(states[i], actions[i][j]);
          auto probs =
              get_action_probs(actions[i][j], transition_probabilities[t][1], op_a,
                               transition_probabilities[t][0]);
          for (int k = 0; k < states.size(); ++k) {
            p[t][i][j][k] = probs[states[k]];
          }
        }
      }
    }
  }

  RMAB() {
    actions = generate_actions(states, alpha_n);
    for (int i = 0; i < states.size(); ++i) {
      state_to_idx[states[i]] = i;
    }
    precompute_rewards();
    precompute_transition_probabilities();
  }

  double find_policy() {
    array<array<double, states.size()>, H> dp;
    memset(dp.data(), -DBL_MAX, sizeof(dp));
  }
};

constexpr int H = 1;
constexpr int N = 10;
constexpr int S = 4;
constexpr double alpha = 0.5;

constexpr array<array<array<double, 2>, S>, H> rewards = {};

constexpr array<array<array<array<double, S>, S>, 2>, H>
    transition_probabilities = {};

constexpr array<int, S> initial_state = {5, 5};

int main() {
  RMAB<H, N, S, alpha, initial_state, rewards, transition_probabilities> rmab;
  auto actions = generate_actions(rmab.states, rmab.alpha_n);
  // compile_time::print_states(rmab.states);

  // constexpr auto states = generate_states<10, 4>();
  // constexpr auto actions = generate_actions<states, 5>();
  // std::cout << actions.size() << std::endl;
  int sum = 0;
  for (const auto &action : rmab.actions) {
    sum += action.size();
  }
  cout << sum << endl;
  return 0;
}