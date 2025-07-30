#include <array>
#include <algorithm>
#include <iostream>
#include <vector>

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

template<size_t N, size_t S>
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

template<size_t N, size_t S>
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

template<size_t S, int alpha_n, size_t actions_size = 0>
consteval auto find_actions(const std::array<int, S> & state) {
    std::vector<std::pair<int, std::vector<int>>> actions = {{0, {}}}, tmp;
    // for (int i = 0; i < alpha_n; ++i) {
    //     tmp.clear();
    //     for (int j = 0; j < std::min(state[i], alpha_n) + 1; ++j) {
    //         for (const auto & action : actions) {
    //             if (action.first + j <= alpha_n) {
    //                 auto new_action = action.second;
    //                 new_action.push_back(j);
    //                 tmp.push_back({action.first + j, new_action});
    //             }
    //         }
    //         actions = std::move(tmp);
    //     }
    // }
    if constexpr (actions_size == 0) {
        return actions.size();
    } else {
        std::array<std::array<int, S>, actions_size> res;
        for (int i = 0; i < actions_size; ++i) {
            for(int j = 0; j < S; ++j) {
                res[i][j] = actions[i].second[j];
            }
        }
        return res;
    }
}

template<int alpha_n, size_t S>
consteval auto generate_actions(const std::array<int, S> & state) {
    constexpr size_t actions_size = find_actions<S, alpha_n>(state);
    return find_actions<S, alpha_n, actions_size>(state);
}

// template<size_t S, size_t n>
// consteval auto generate_actions(const std::array<std::array<int, S>, n> & states) {
    
// }

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