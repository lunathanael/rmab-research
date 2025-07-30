#include <array>
#include <algorithm>
#include <iostream>

consteval int combinations(int n, int r) {
    if (r > n || r < 0 || n < 0) return 0;
    if (r == 0 || r == n) return 1;
    
    long long result = 1;
    r = std::min(r, n - r);
    
    for (int i = 0; i < r; ++i) {
        result = result * (n - i) / (i + 1);
    }
    return static_cast<int>(result);
}

template<int N, int S>
consteval void fill_states_helper(auto& states, int& idx, std::array<int, S>& current, int pos, int remaining) {
    if (pos == S - 1) {
        current[pos] = remaining;
        states[idx++] = current;
        return;
    }
    
    for (int i = 0; i <= remaining; ++i) {
        current[pos] = i;
        fill_states_helper<N, S>(states, idx, current, pos + 1, remaining - i);
    }
}

template<int N, int S>
consteval auto generate_states() {
    constexpr int n = combinations(N + S - 1, S - 1);
    using state_t = std::array<int, S>;
    using state_array_t = std::array<state_t, n>;
    
    state_array_t states{};
    int idx = 0;
    state_t current{};
    
    fill_states_helper<N, S>(states, idx, current, 0, N);
    
    return states;
}

template<typename T, size_t S, size_t n>
void print_states(const std::array<std::array<T, S>, n>& states) {
    for (const auto& state : states) {
        std::cout << "[";
        for (size_t i = 0; i < state.size(); ++i) {
            std::cout << state[i];
            if (i < state.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
}