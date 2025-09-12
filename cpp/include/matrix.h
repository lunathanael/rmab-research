#pragma once

#include <iostream>
#include <vector>

class transition_matrix_t : public std::vector<std::vector<double>> {
public:
  friend std::istream &operator>>(std::istream &is,
                                  transition_matrix_t &matrix);
  friend std::ostream &operator<<(std::ostream &os,
                                  const transition_matrix_t &matrix);
};