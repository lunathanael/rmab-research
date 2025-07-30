#include <bits/stdc++.h>
using namespace std;

consteval int combinations(int n, int r) {
    int ans = 1;
    for (int i = n; i > n-r; i--) {
        ans *= i;
    }
    for (int i = 1; i <= r; i++) {
        ans /= i;
    }
    return ans;
}

template<int N, int S>
consteval auto generate_states() {
    constexpr int n = combinations(N+S-1, S-1);
    using state_t = array<int, S>;
    using state_array_t = array<state_t, n>;
    state_array_t states;

    if constexpr (S == 1) {
        states[0] = {N};
        return states;
    }
    
    int idx = 0;
    for(int i = 0; i <= N; ++i) {
        for (const auto & a : generate_states<N-i, S-1>()) {
            for (int j = 0; j < S; ++j) {
                states[idx][j + 1] = a[j];
                ++idx;
            }
            states[idx++][0] = i;
        }
    }
    return states;
}

template <
    int H, int N, int S, double alpha, array<int, S> initial_state,
    array<array<array<double, 2>, S>, H> rewards,
    array<array<array<array<double, 2>, S>, S>, H> transition_probabilities>
class RMAB {
    
public:
};


int main() {
    constexpr auto states = generate_states<10, 2>();
    return 0;
}