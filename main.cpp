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
    constexpr auto states = generate_states<10, 4>();
    constexpr auto actions = generate_actions<states, 5>();
    std::cout << actions.size() << std::endl;
    for(const auto & action : actions) {
        print_states(action);
    }
    return 0;
}