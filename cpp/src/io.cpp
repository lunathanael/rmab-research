#include "rmab.h"

#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

void RMAB::parse() {
  cin >> n_arms >> n_steps >> n_states >> alpha;
  n_alpha = n_arms * alpha;
  if (n_alpha / alpha != n_arms) {
    std::cerr << "Warning: Integer floor precision error detected in n_alpha "
                 "calculation\n";
  }
  initial_state.resize(n_states);

  for (int i = 0; i < n_states; ++i) {
    double prop;
    cin >> prop;
    int n = n_arms * prop;
    if (n / prop != n_arms) {
      std::cerr << "Warning: Integer floor precision error detected in initial "
                   "state calculation\n";
    }
    initial_state[i] = n;
  }

  if (std::accumulate(initial_state.begin(), initial_state.end(), 0) !=
      n_arms) {
    std::cerr << "Warning: Initial state does not sum to number of arms\n";
  }

  rewards.resize(n_steps, vector<pair<double, double>>(n_states));
  for (int t = 0; t < n_steps; ++t)
    for (int s = 0; s < n_states; ++s)
      cin >> rewards[t][s].first >> rewards[t][s].second;

  transition_probabilities.resize(n_steps);
  for (int t = 0; t < n_steps; ++t) {
    transition_probabilities[t].first.resize(n_states,
                                             vector<double>(n_states));
    transition_probabilities[t].second.resize(n_states,
                                              vector<double>(n_states));
    cin >> transition_probabilities[t].first >>
        transition_probabilities[t].second;
  }
}

void RMAB::print_args() {
  cout << format("Number of arms: {}, Number of time steps: {}, Number of "
                 "states: {}, Alpha: {}\n",
                 n_arms, n_steps, n_states, alpha);
  cout << "Initial state:\n";
  for (int i = 0; i < n_states; ++i)
    cout << setw(4) << initial_state[i] << ' ';
  cout << '\n';

  cout << "Rewards:\n";
  for (int t = 0; t < n_steps; ++t) {
    cout << format("r{}(· | ·)=\n", t);
    for (int s = 0; s < n_states; ++s) {
      cout << setw(5) << rewards[t][s].first << setw(5) << rewards[t][s].second << '\n';
    }
  }

  cout << "Transition Probabilities:\n";
  for (int t = 0; t < n_steps; ++t) {
    cout << format("P{}(· | ·, 0)=\n", t);
    cout << transition_probabilities[t].first;
    cout << format("P{}(· | ·, 1)=\n", t);
    cout << transition_probabilities[t].second;
  }
  cout << '\n';
}