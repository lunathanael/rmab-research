#include "matrix.h"

#include <iomanip>

using namespace std;

std::istream &operator>>(std::istream &is, transition_matrix_t &matrix) {
  int n_states = matrix.size();
  for (int s_before = 0; s_before < n_states; ++s_before) {
    for (int s_after = 0; s_after < n_states; ++s_after) {
      is >> matrix[s_before][s_after];
    }
  }
  return is;
}

std::ostream &operator<<(std::ostream &os, const transition_matrix_t &matrix) {
  int n_states = matrix.size();
  os << std::fixed << std::setprecision(4);
  for (int s_before = 0; s_before < n_states; ++s_before) {
    for (int s_after = 0; s_after < n_states; ++s_after) {
      os << std::setw(8) << matrix[s_before][s_after];
    }
    os << '\n';
  }
  return os;
}