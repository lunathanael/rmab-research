#include <bits/stdc++.h>
#include "utils.h"

using namespace std;

template <
    int H, int N, int S, double alpha, array<int, S> initial_state,
    array<array<array<double, 2>, S>, H> rewards,
    array<array<array<array<double, 2>, S>, S>, H> transition_probabilities>
class RMAB {
    static constexpr const int alpha_n = alpha * N;
    static_assert(alpha_n / alpha == N, "alpha * N must be an integer");
    static constexpr const auto states = generate_states<N, S>();
public:
};


int main() {
    constexpr auto states = generate_states<50, 4>();
    // print_states(states);
    constexpr auto actions = generate_actions<10>(states[0]);
    // print_actions(actions);
    return 0;
}