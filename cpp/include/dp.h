#pragma once

#include <vector>
#include <cstdint>

class DPStateIterator {
public:
  DPStateIterator(int n_arms, int n_states);
  void init();
  const std::vector<int> &current() const;
  std::uint64_t current_hash() const;
  int size() const;
  bool next();

private:
  std::vector<int> comp;
  const int n;
  int n_arms, n_states;
  int lastNonZero, prefixSum;
  bool done;

public:
  int idx;
};

class StateActionIterator {
  long long max_right_capacity(int start_idx) const;

public:
  StateActionIterator(const DPStateIterator &it, int n_alpha);
  const std::vector<int> &current() const;
  bool next();

private:
  std::vector<int> x, y;
  long long sum_y;
  int n_states, n_alpha;
  bool done, started;
};

class DPLayer {
  std::vector<double> dp;
  int n_arms, n_states;
  int dim;

public:
  DPLayer(int n_arms, int n_states);
  double &operator[](const DPStateIterator &dpstate);
  friend void swap(DPLayer &a, DPLayer &b) noexcept;
};