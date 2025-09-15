#pragma once

#include "prob.h"
#include "utils.h"

#include <cstdint>
#include <vector>

class DPStateIterator {
public:
  DPStateIterator(int n_arms, int n_states);
  void init();
  const std::vector<int> &current() const;
  bool next();

private:
  std::vector<int> comp;
  const std::uint64_t n;
  int n_arms, n_states;
  int lastNonZero, prefixSum;
  bool done;

public:
  std::uint64_t idx;
};

class DPState : public BitArray {
public:
  std::uint64_t relative_idx;
  DPState(const std::vector<int> &v, std::uint64_t relative_idx)
      : BitArray(v), relative_idx{relative_idx} {}
  DPState(const BitArray &x, int n_arms, int n_states);
};

class DPStates {
  std::vector<DPState> dp_states;
  const std::uint64_t n;
  const int n_arms, n_states;

public:
  DPStates(int n_arms, int n_states);
  const DPState &operator[](std::uint64_t idx) const { return dp_states[idx]; }
  std::uint64_t size() const { return n; }
};

class StateActionIterator {
  long long max_right_capacity(int start_idx) const;

public:
  StateActionIterator(const BitArray &state, int n_states, int n_alpha);
  const std::vector<int> &current() const;
  bool next();

private:
  std::vector<int> y;
  const BitArray &x;
  long long sum_y;
  int n_states, n_alpha;
  bool done, started;
};

struct DPValue {
  double expectation;
  BitArray best_action;
  DPValue(double val) : expectation{val} {}
  DPValue(double expectation, BitArray best_action)
      : expectation{expectation}, best_action{best_action} {}
};

class DPLayer {
  std::vector<DPValue> dp;
  int n_arms, n_states;
  int dim;

public:
  DPLayer(int n_arms, int n_states);
  DPValue &operator[](const DPState &dpstate);
  int size() const;
  friend void swap(DPLayer &a, DPLayer &b) noexcept;
};