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
  array<array<vector<array<double, states.size()>>, states.size()>, 1> p;
  array<array<pair<double, array<int, S> *>, states.size()>, H> dp;

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
    cout << "allocating p" << endl;
    for (int t = 0; t < 1; ++t) {
        for (int i = 0; i < states.size(); ++i) {
          p[t][i].reserve(actions[i].size());
          p[t][i].resize(actions[i].size());
        }
    }
    cout << "precomputing p" << endl;
    for (int t = 0; t < 1; ++t) {
      for (int i = 0; i < states.size(); ++i) {
        for (int j = 0; j < actions[i].size(); ++j) {
          auto op_a = subtract_array(states[i], actions[i][j]);
          auto probs =
              get_action_probs<N>(actions[i][j], transition_probabilities[t][1],
                               op_a, transition_probabilities[t][0]);
          for (int k = 0; k < states.size(); ++k) {
            p[t][i][j][k] = probs[states[k]];
          }
        }
      }
    }
    // for (int t = 1; t < H; ++t) {
    //   for (int i = 0; i < states.size(); ++i) {
    //     p[t][i] = p[t - 1][i];
    //   }
    // }
  }

  RMAB() {
    cout << "Precomputing actions" << endl;
    actions = generate_actions(states, alpha_n);
    for (int i = 0; i < states.size(); ++i) {
      state_to_idx[states[i]] = i;
    }
    cout << "Precomputing rewards" << endl;
    precompute_rewards();
    // cout << "Precomputing transition probabilities" << endl;
    // precompute_transition_probabilities();
    cout << "Done initializing" << endl;
  }

  static auto print_array(array<int, S> *arr) {
    if (arr == nullptr) {
      cout << "(nullptr)" << endl;
      return;
    }
    cout << "(";
    for (int i = 0; i < S; ++i) {
      cout << (*arr)[i];
      if (i != S - 1) {
        cout << ", ";
      }
    }
    cout << ")";
  }

  void print_policy() {
    for (int t = 0; t < H; ++t) {
      cout << "t = " << t << endl;
      for (int i = 0; i < states.size(); ++i) {
        cout << "{" << dp[t][i].first << ", ";
        print_array(dp[t][i].second);
        cout << "} \n";
      }
    }
  }

  double find_policy() {
    for (int i = 0; i < H; ++i) {
      for (int j = 0; j < states.size(); ++j) {
        dp[i][j] = {-DBL_MAX, nullptr};
      }
    }
    for (int t = H - 1; t >= 0; --t) {
      for (int i = 0; i < states.size(); ++i) {
        for (int j = 0; j < actions[i].size(); ++j) {
          double reward = r[t][i][j];
          auto op_a = subtract_array(states[i], actions[i][j]);
          auto probs =
              get_action_probs<N>(actions[i][j], transition_probabilities[0][1],
                               op_a, transition_probabilities[0][0]);
          for (int k = 0; k < states.size(); ++k) {
            if (t != H - 1) {
            //   reward += p[0][i][j][k] * dp[t + 1][k].first;
                reward += probs[states[k]] * dp[t + 1][k].first;
            }
            if (reward > dp[t][i].first) {
              dp[t][i] = {reward, &actions[i][j]};
            }
          }
        }
      }
    }
    print_policy();
    return dp[0][state_to_idx[initial_state]].first;
  }
};

constexpr int H = 3;
constexpr int N = 50;
constexpr int S = 4; // 4 states based on the matrices
constexpr double alpha = 0.4;


// rewards[h][state][action] - from the r matrix
constexpr array<array<array<double, 2>, 4>, H> rewards = {
    {// H = 1 (index 0) - values constant across time
     {{
         {0.44926316, 0.10939499}, // state 0
         {0.21953011, 0.85231770}, // state 1
         {0.10236761, 0.06171567}, // state 2
         {0.27516813, 0.97393949}  // state 3
     }},
     // H = 2 (index 1) - same values (constant in time)
     {{
         {0.44926316, 0.10939499}, // state 0
         {0.21953011, 0.85231770}, // state 1
         {0.10236761, 0.06171567}, // state 2
         {0.27516813, 0.97393949}  // state 3
     }},
     {{
         {0.44926316, 0.10939499}, // state 0
         {0.21953011, 0.85231770}, // state 1
         {0.10236761, 0.06171567}, // state 2
         {0.27516813, 0.97393949}  // state 3
     }}}};

// transition_probabilities[h][action][current_state][next_state]
constexpr array<array<array<array<double, 4>, 4>, 2>, H>
    transition_probabilities = {
        {  // H = 1 (index 0) - values constant across time
         {{// action = 0: P(·|·,0) matrix
           {{
               {0.70317263, 0.29682737, 0.00000000, 0.00000000}, // from state 0
               {0.00000000, 0.03159578, 0.00000000, 0.96840422}, // from state 1
               {0.00000000, 0.00000000, 0.37932461, 0.62067539}, // from state 2
               {0.30200956, 0.00000000, 0.00000000, 0.69799044}  // from state 3
           }},
           // action = 1: P(·|·,1) matrix
           {{
               {0.00000000, 0.00000000, 0.08423799, 0.91576201}, // from state 0
               {0.80689287, 0.00000000, 0.19310713, 0.00000000}, // from state 1
               {0.20350136, 0.79649864, 0.00000000, 0.00000000}, // from state 2
               {0.00000000, 0.16178167, 0.00000000, 0.83821833}  // from state 3
           }}}},
         // H = 2 (index 1) - same probabilities (constant in time)
         {{// action = 0: P(·|·,0) matrix
           {{
               {0.70317263, 0.29682737, 0.00000000, 0.00000000}, // from state 0
               {0.00000000, 0.03159578, 0.00000000, 0.96840422}, // from state 1
               {0.00000000, 0.00000000, 0.37932461, 0.62067539}, // from state 2
               {0.30200956, 0.00000000, 0.00000000, 0.69799044}  // from state 3
           }},
           // action = 1: P(·|·,1) matrix
           {{
               {0.00000000, 0.00000000, 0.08423799, 0.91576201}, // from state 0
               {0.80689287, 0.00000000, 0.19310713, 0.00000000}, // from state 1
               {0.20350136, 0.79649864, 0.00000000, 0.00000000}, // from state 2
               {0.00000000, 0.16178167, 0.00000000, 0.83821833}  // from state 3
           }}}},

         // H = 2 (index 1) - same probabilities (constant in time)
         {{// action = 0: P(·|·,0) matrix
           {{
               {0.70317263, 0.29682737, 0.00000000, 0.00000000}, // from state 0
               {0.00000000, 0.03159578, 0.00000000, 0.96840422}, // from state 1
               {0.00000000, 0.00000000, 0.37932461, 0.62067539}, // from state 2
               {0.30200956, 0.00000000, 0.00000000, 0.69799044}  // from state 3
           }},
           // action = 1: P(·|·,1) matrix
           {{
               {0.00000000, 0.00000000, 0.08423799, 0.91576201}, // from state 0
               {0.80689287, 0.00000000, 0.19310713, 0.00000000}, // from state 1
               {0.20350136, 0.79649864, 0.00000000, 0.00000000}, // from state 2
               {0.00000000, 0.16178167, 0.00000000, 0.83821833}  // from state 3
           }}}}}};

constexpr array<int, S> initial_state = {20, 15, 15, 0};

int main() {
  RMAB<H, N, S, alpha, initial_state, rewards, transition_probabilities> rmab;
  double res = rmab.find_policy();
  cout << res << endl;
  cout << res / N << endl;
  return 0;
}