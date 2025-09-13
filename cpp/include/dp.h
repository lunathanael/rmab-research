#pragma once

#include <vector>

class DPStateIterator {
public:
  DPStateIterator(int n_arms, int n_states);
  void init();
  const std::vector<int> &current() const;
  int next();
  bool done() const;
  int size() const;

private:
  std::vector<int> comp;
  const int n;
  int n_arms, n_states;
  int lastNonZero;

public:
  int idx;
};

class StateActionIterator {
public:
  StateActionIterator(const DPStateIterator &it, int n_alpha);
  const std::vector<int> &current() const;
  bool next();

private:
  std::vector<int> x, y;
  int n_states, n_alpha, sum_y;
  bool done, started = false;
};

class DPLayer {
  std::vector<double> dp;
  int n_arms, n_states;
  int dim;

public:
  DPLayer(int n_states, int n_arms);
  double &operator[](const DPStateIterator &dpstate);
  friend void swap(DPLayer &a, DPLayer &b) noexcept;
};