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
  array<
      array<vector<array<double, states.size()>>, states.size()>,
      H>
      p;

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
        p[t][i].resize(actions[i].size());
        for (int j = 0; j < actions[i].size(); ++j) {
          auto op_a = subtract_array(states[i], actions[i][j]);
          auto probs =
              get_action_probs(actions[i][j], transition_probabilities[t][1],
                               op_a, transition_probabilities[t][0]);
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
    for(int i = 0; i < H; ++i) {
      for(int j = 0; j < states.size(); ++j) {
        dp[i][j] = -DBL_MAX;
      }
    }
    for (int t = H - 1; t >= 0; --t) {
      for (int i = 0; i < states.size(); ++i) {
        for (int j = 0; j < actions[i].size(); ++j) {
          double reward = r[t][i][j];
          for (int k = 0; k < states.size(); ++k) {
            if (t != H - 1) {
              reward += p[t][i][j][k] * dp[t + 1][k];
            }
            if (reward > dp[t][i]) {
              dp[t][i] = reward;
            }
          }
        }
      }
    }
    return dp[0][state_to_idx[initial_state]];
  }
};

constexpr int H = 2;
constexpr int N = 120;
constexpr int S = 2;  // 2 arms based on initial_state = (60, 60)
constexpr double alpha = 0.5;

// rewards[h][state][action] - only state 0, action 1 gives reward 1
constexpr array<array<array<double, 2>, 2>, H> rewards = {{
    // H = 1 (index 0)
    {{
        {0.0, 1.0}, // state 0: action 0 = 0, action 1 = 1
        {0.0, 0.0}  // state 1: action 0 = 0, action 1 = 0
    }},
    // H = 2 (index 1) 
    {{
        {0.0, 1.0}, // state 0: action 0 = 0, action 1 = 1
        {0.0, 0.0}  // state 1: action 0 = 0, action 1 = 0
    }}
}};

// transition_probabilities[h][action][current_state][next_state]
constexpr array<array<array<array<double, 2>, 2>, 2>, H> transition_probabilities = {{
    // H = 1 (index 0)
    {{
        // action = 0
        {{
            {0.9, 0.1}, // current_state 0: P(0->0)=0.9, P(0->1)=0.2  
            {0.25, 0.75} // current_state 1: P(1->0)=0.25, P(1->1)=0.7
        }},
        // action = 1  
        {{
            {0.2, 0.8}, // current_state 0: P(0->0)=0.2, P(0->1)=0.8
            {0.7, 0.3} // current_state 1: P(1->0)=0.75, P(1->1)=0.25
        }}
    }},
    // H = 2 (index 1) - same probabilities
    {{
        // action = 0
        {{
            {0.9, 0.2}, // current_state 0: P(0->0)=0.9, P(0->1)=0.2  
            {0.25, 0.7} // current_state 1: P(1->0)=0.25, P(1->1)=0.7
        }},
        // action = 1  
        {{
            {0.1, 0.8}, // current_state 0: P(0->0)=0.1, P(0->1)=0.8
            {0.75, 0.3} // current_state 1: P(1->0)=0.75, P(1->1)=0.3
        }}
    }}
}};

constexpr array<int, S> initial_state = {60, 60};

int main() {
  RMAB<H, N, S, alpha, initial_state, rewards, transition_probabilities> rmab;
  auto actions = generate_actions(rmab.states, rmab.alpha_n);
  cout << actions.size() << endl;
  double res = rmab.find_policy();
  cout << res << endl;
  cout << res / N << endl;
  return 0;
}