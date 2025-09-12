#pragma once

#include "matrix.h"

#include <vector>

class RMAB {
  std::vector<double> initial_state;
  std::vector<std::vector<std::pair<double, double>>> rewards;
  std::vector<std::pair<transition_matrix_t, transition_matrix_t>>
      transition_probabilities;
  double alpha;
  int n_arms, n_steps, n_states;

public:
  void parse();
  void print_args();
};