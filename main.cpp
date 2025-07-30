#include <bits/stdc++.h>
#include "utils.h"

using namespace std;

template <
    int H, int N, int S, double alpha, array<int, S> initial_state,
    array<array<array<double, 2>, S>, H> rewards,
    array<array<array<array<double, 2>, S>, S>, H> transition_probabilities>
class RMAB {
public:
    static constexpr const int alpha_n = alpha * N;
    static_assert(alpha_n / alpha == N, "alpha * N must be an integer");
    static constexpr const auto states = compile_time::generate_states<N, S>();
    std::array<std::vector<std::array<int, S>>, states.size()> actions;

    RMAB() {
        actions = generate_actions(states, alpha_n);
    }
    // static constexpr const auto actions = generate_actions1(states, alpha_n);
};

constexpr int H = 1;
constexpr int N = 10;
constexpr int S = 4;
constexpr double alpha = 0.5;

constexpr array<array<array<double, 2>, S>, H> rewards = {};

constexpr array<array<array<array<double, 2>, S>, S>, H> transition_probabilities = {};

constexpr array<int, S> initial_state = {5, 5};



int main() {
    RMAB<H, N, S, alpha, initial_state, rewards, transition_probabilities> rmab;
    auto actions = generate_actions(rmab.states, rmab.alpha_n);
    // compile_time::print_states(rmab.states);

    // constexpr auto states = generate_states<10, 4>();
    // constexpr auto actions = generate_actions<states, 5>();
    // std::cout << actions.size() << std::endl;
    int sum = 0;
    for(const auto & action : rmab.actions) {
        sum += action.size();
    }
    cout << sum << endl;
    return 0;
}