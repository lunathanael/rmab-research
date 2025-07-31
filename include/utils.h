#include <array>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>

template<typename T>
struct is_std_array : std::false_type {};

template<typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

namespace compile_time {
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

template<auto state, int alpha_n, size_t actions_size = 0>
consteval auto find_actions() {
    static_assert(is_std_array<decltype(state)>::value);
    constexpr size_t S = state.size();
    static_assert(std::is_same_v<decltype(state), decltype(std::array<int, S>())>);

    std::vector<std::pair<int, std::vector<int>>> actions = {{0, {}}}, tmp;
    for (int i = 0; i < S - 1; ++i) {
        tmp.clear();
        for (int j = 0; j < std::min(state[i], alpha_n) + 1; ++j) {
            for (const auto & action : actions) {
                if (action.first + j <= alpha_n) {
                    auto new_action = action.second;
                    new_action.push_back(j);
                    tmp.push_back({action.first + j, new_action});
                }
            }
        }
        actions = std::move(tmp);
    }
    tmp.clear();
    for(const auto & action : actions) {
        int j = alpha_n - action.first;
        if (j <= state.back()) {
            auto new_action = action.second;
            new_action.push_back(j);
            tmp.push_back({action.first + j, new_action});
        }
    }
    actions = std::move(tmp);
    if constexpr (actions_size == 0) {
        return actions.size();
    } else {
        std::array<std::array<int, S>, actions_size> res;
        for (int i = 0; i < actions.size(); ++i) {
            for(int j = 0; j < S; ++j) {
                res[i][j] = actions[i].second[j];
            }
        }
        return res;
    }
}

template<auto states, int alpha_n, size_t... I>
consteval auto count_actions_impl(std::index_sequence<I...>) {
    return std::array<size_t, sizeof...(I)>{find_actions<states[I], alpha_n>()...};
}

template<auto states, int alpha_n>
consteval auto count_actions() {
    static_assert(is_std_array<decltype(states)>::value);
    constexpr size_t n = states.size();
    return count_actions_impl<states, alpha_n>(std::make_index_sequence<n>());
}

template<auto states, int alpha_n, size_t max_actions, size_t... I>
consteval auto set_actions_impl(std::index_sequence<I...>) {
    static_assert(is_std_array<decltype(states)>::value);
    static_assert(is_std_array<typename decltype(states)::value_type>::value);
    constexpr size_t S = std::tuple_size_v<typename decltype(states)::value_type>;
    
    // Debug: Check what find_actions actually returns
    using ExpectedType = std::array<std::array<int, S>, max_actions>;
    using ActualType = decltype(find_actions<states[0], alpha_n, max_actions>());
    static_assert(std::is_same_v<ExpectedType, ActualType>, 
                  "find_actions return type mismatch");
    
    return std::array<ExpectedType, sizeof...(I)>{
        find_actions<states[I], alpha_n, max_actions>()...
    };
}

template<auto states, int alpha_n>
consteval auto generate_actions() {
    static_assert(is_std_array<decltype(states)>::value);
    constexpr size_t n = states.size();
    static_assert(is_std_array<typename decltype(states)::value_type>::value);
    constexpr size_t S = std::tuple_size_v<typename decltype(states)::value_type>;
    constexpr auto action_counts = count_actions<states, alpha_n>();
    constexpr size_t max_actions = *std::max_element(action_counts.begin(), action_counts.end());
    return set_actions_impl<states, alpha_n, max_actions>(std::make_index_sequence<n>());
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
}

template<size_t S>
constexpr auto find_actions(const std::array<int, S> &state, int alpha_n) {
    std::vector<std::pair<int, std::array<int, S>>> actions = {{0, {}}}, tmp;
    for (int i = 0; i < S - 1; ++i) {
        tmp.clear();
        for (int j = 0; j < std::min(state[i], alpha_n) + 1; ++j) {
            for (const auto & action : actions) {
                if (action.first + j <= alpha_n) {
                    auto new_action = action.second;
                    new_action[i] = j;
                    tmp.push_back({action.first + j, new_action});
                }
            }
        }
        actions = std::move(tmp);
    }
    tmp.clear();
    for(const auto & action : actions) {
        int j = alpha_n - action.first;
        if (j <= state.back()) {
            auto new_action = action.second;
            new_action[S - 1] = j;
            tmp.push_back({action.first + j, new_action});
        }
    }
    actions = std::move(tmp);
    std::vector<std::array<int, S>> res(actions.size());
    for (int i = 0; i < actions.size(); ++i) {
        res[i] = actions[i].second;
    }
    return res;
}

template<size_t S, size_t n>
constexpr auto generate_actions(const std::array<std::array<int, S>, n> &states, int alpha_n) {
    std::array<std::vector<std::array<int, S>>, n> action_counts;
    for (size_t i = 0; i < n; ++i) {
        action_counts[i] = find_actions(states[i], alpha_n);
    }
    return action_counts;
}

hash_array<int>{}<int>()

template <size_t N> struct hash_array {
    template <typename T, size_t S>
    size_t operator()(const std::array<T, S> &arr) const {
      size_t res = 0;
      size_t scalar = 1;
      for (int i = 0; i < S; ++i) {
        res += arr[i] * scalar;
        scalar *= N + 1;
      }
      return res;
    }
  };

  
template<size_t S>
constexpr void fill_states_helper(auto& states, int& idx, std::array<int, S>& current, int pos, int remaining) {
    if (pos == S - 1) {
        current[pos] = remaining;
        states[idx++] = current;
        return;
    }
    
    for (int i = 0; i <= remaining; ++i) {
        current[pos] = i;
        fill_states_helper<S>(states, idx, current, pos + 1, remaining - i);
    }
}


constexpr int combinations(int n, int r) {
    if (r > n || r < 0 || n < 0) return 0;
    if (r == 0 || r == n) return 1;
    
    long long result = 1;
    r = std::min(r, n - r);
    
    for (int i = 0; i < r; ++i) {
        result = result * (n - i) / (i + 1);
    }
    return static_cast<int>(result);
}

template<size_t S>
auto generate_states(std::size_t N) {
    size_t n = combinations(N + S - 1, S - 1);
    using state_t = std::array<int, S>;
    using state_array_t = std::vector<state_t>;
    
    state_array_t states{n};
    int idx = 0;
    state_t current{};
    
    fill_states_helper<S>(states, idx, current, 0, N);
    
    return states;
}

template<size_t S, size_t N>
auto generate_all_states() {
    std::array<std::vector<std::array<int, S>>, N> states;
    for(int i = 0; i < N; ++i) {
        states[i] = generate_states<S>(i);
    }
    return states;
}


template<size_t S>
constexpr auto add_array(const std::array<int, S> &a, const std::array<int, S> &b) {
    std::array<int, S> res;
    for(int i = 0; i < S; ++i) {
        res[i] = a[i] + b[i];
    }
    return res;
}

template<size_t S>
constexpr auto subtract_array(const std::array<int, S> &a, const std::array<int, S> &b) {
    std::array<int, S> res;
    for(int i = 0; i < S; ++i) {
        res[i] = a[i] - b[i];
    }
    return res;
}

template<size_t S, size_t N>
constexpr auto int_to_goal(const uint64_t i) {
    std::array<int, S> goal;
    uint64_t scalar = 1;
    for(int j = 0; j < S; ++j) {
        goal[j] = (i / scalar) % (N + 1);
        scalar *= N + 1;
    }
    return goal;
}

template<size_t S, size_t N>
constexpr auto goal_to_int(const std::array<int, S> &goal) {
    uint64_t res = 0;
    uint64_t scalar = 1;
    for(int j = S-1; j >= 0; --j) {
        res += goal[j] * scalar;
        scalar *= N + 1;
    }
    return res;
}

template<size_t S, size_t N>
auto generate_all_goals() {
    static constexpr size_t size_of_goals = static_cast<uint64_t>(std::pow(N + 1, S));
    std::vector<std::array<int, S>> goals(size_of_goals);

    for(uint64_t i = 0; i < static_cast<uint64_t>(std::pow(N + 1, S)); ++i) {
        auto goal = int_to_goal<S, N>(i);
        goals[i] = goal;
    }

    return goals;
}


template< size_t N, size_t S>
auto calculate_coefficients() {
    static auto goals = generate_all_goals<S, N>();
    std::array<std::vector<double>, N + 1> coefficients;
    for(int x = 0; x <= N; ++x) {
        coefficients[x].reserve(goals.size());
        coefficients[x].resize(goals.size());
        for(int i = 0; i < goals.size(); ++i) {
            auto goal = goals[i];
            double res = 1;
            for(int i = 1; i <= x; ++i) {
                res *= i;
                for(int j = 0; j < S; ++j) {
                    if(goal[j] >= i) {
                        res /= i;
                    }
                }
            }
            coefficients[x][i] = res;
        }
    }
    return coefficients;
}

template<size_t N, size_t S>
constexpr double multinomial(int x, const std::array<int, S> &goal, const std::array<double, S> &probs) {
    static auto coefficients = calculate_coefficients<N, S>();
    const auto goal_int = goal_to_int<S, N>(goal);

    double res = coefficients[x][goal_int];
    for(int i = 0; i < S; ++i) {
        res *= std::pow(probs[i], goal[i]);
    }
    return res;
}

template<size_t N, size_t S>
auto get_action_probs(const std::array<int, S> &count_0, const std::array<std::array<double, S>, S> &probs_0, const std::array<int, S> &count_1, const std::array<std::array<double, S>, S> &probs_1) {
    static const auto states = generate_all_states<S, N>();
    static constexpr size_t number_of_goals = std::pow(N + 1, S);

    std::array<double, number_of_goals> dp, tmp;
    dp[{}] = 1;

    for(int i = 0; i < S; ++i) {
        tmp.clear();
        const auto &allocations = states[count_0[i]];
        for(const auto & allocation : allocations) {
            double prob = multinomial<N>(count_0[i], allocation, probs_0[i]);
            for(const auto & [key, value] : dp) {
                auto new_key = add_array(key, allocation);
                tmp[hash_array<N>(new_key)] += value * prob;
            }
        }
        dp = std::move(tmp);
    }

    for(int i = 0; i < S; ++i) {
        tmp.clear();
        const auto &allocations = states[count_1[i]];
        for(const auto & allocation : allocations) {
            double prob = multinomial<N>(count_1[i], allocation, probs_1[i]);
            for(const auto & [key, value] : dp) {
                auto new_key = add_array(key, allocation);
                tmp[hash_array<N>(new_key)] += value * prob;
            }
        }
        dp = std::move(tmp);
    }
    return dp;
}