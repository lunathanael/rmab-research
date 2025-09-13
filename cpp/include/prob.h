#pragma once

#include <cstdint>
#include <map>
#include <vector>

class State {
  std::uint64_t val;
  const int n_arms, n_states;

public:
  State(int n_arms, int n_states);
  State(const std::vector<int> &v, int n_arms);

  std::vector<int> to_vector() const;

  int operator[](int i) const;

  void assign(int n, int x);
  void assign(const std::vector<int> &v);
  void set_at(int idx, int value);

  bool empty() const;
  size_t size() const;

  State operator+(const State &other) const;
  State operator+(int x) const;

  bool operator<(const State &other) const;
  bool operator==(const State &other) const;

  struct ConstStateIt;
  ConstStateIt begin() const;
  ConstStateIt end() const;

  operator uint64_t() const { return val; }
};
using ProbDist = std::map<std::uint64_t, double>;

void multinomialDistribution(int n, const std::vector<double> &probs,
                             ProbDist &out, State current, int idx = 0);

ProbDist convolve(const ProbDist &a, const ProbDist &b);

ProbDist fullTransitionDistribution(const std::vector<int> &S,
                                    const std::vector<int> &A,
                                    const std::vector<std::vector<double>> &p0,
                                    const std::vector<std::vector<double>> &p1);
