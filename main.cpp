#include <bits/stdc++.h>
#include "utils.h"

using namespace std;

template <
    int H, int N, int S, double alpha, array<int, S> initial_state,
    array<array<array<double, 2>, S>, H> rewards,
    array<array<array<array<double, 2>, S>, S>, H> transition_probabilities>
class RMAB {
    
public:
};


int main() {
    constexpr auto states = generate_states<10, 3>();
    print_states(states);
    return 0;
}