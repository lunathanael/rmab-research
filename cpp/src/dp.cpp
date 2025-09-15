#include "dp.h"
#include "prob.h"
#include "utils.h"
#include <algorithm>
#include <numeric>

using namespace std;

DPStates::DPStates(int n_arms, int n_states)
    : n{combinationsull(n_arms + n_states - 1, n_states - 1)}, n_arms(n_arms),
      n_states(n_states) {
  dp_states.reserve(n);
  DPStateIterator sit(n_arms, n_states);
  sit.init();

  for (int i = 0; i < n; ++i) {
    dp_states.emplace_back(sit.current(), sit.idx);
    sit.next();
  }
}

DPStateIterator::DPStateIterator(int n_arms, int n_states)
    : n{combinationsull(n_arms + n_states - 1, n_states - 1)}, n_arms(n_arms),
      n_states(n_states) {
  init();
}

void DPStateIterator::init() {
  comp.assign(n_states, 0);
  comp.back() = n_arms;
  idx = 0;
}

const vector<int> &DPStateIterator::current() const { return comp; }

bool DPStateIterator::next() {
  if (++idx >= n)
    return false;

  int i_pivot = n_states - 1;
  while (i_pivot >= 0 && comp[i_pivot] == 0) {
    i_pivot--;
  }
  comp[i_pivot]--;
  comp[i_pivot - 1]++;
  int remainder = comp[i_pivot];
  comp[i_pivot] = 0;
  comp[n_states - 1] += remainder;
  return true;
}

long long StateActionIterator::max_right_capacity(int start_idx) const {
  long long capacity = 0;
  for (int k = start_idx; k < n_states; ++k) {
    capacity += x[k];
  }
  return capacity;
}

StateActionIterator::StateActionIterator(const BitArray &state, int n_states,
                                         int n_alpha)
    : x(state), n_states(n_states), n_alpha(n_alpha), done(false),
      started{false} {
  y.assign(n_states, 0);
  sum_y = 0;
}

const vector<int> &StateActionIterator::current() const { return y; }

bool StateActionIterator::next() {
  if (done) {
    return false;
  }

  if (!started) {
    started = true;
    int remaining_to_assign = n_alpha;
    for (int i = n_states - 1; i >= 0; --i) {
      int val_for_i = std::min(x[i], remaining_to_assign);
      y[i] = val_for_i;
      remaining_to_assign -= val_for_i;
    }

    if (remaining_to_assign > 0) {
      done = true;
      return false;
    }
    sum_y = n_alpha;
    return true;
  }

  for (int i = n_states - 1; i >= 0; --i) {
    long long prefix_sum_up_to_i_minus_1 = 0;
    if (i > 0) {
      prefix_sum_up_to_i_minus_1 =
          std::accumulate(y.begin(), y.begin() + i, 0LL);
    }

    long long max_allowed_for_y_i_to_reach =
        n_alpha - prefix_sum_up_to_i_minus_1;

    long long proposed_y_i_val = y[i] + 1;
    long long remaining_needed_for_right_states =
        n_alpha - (prefix_sum_up_to_i_minus_1 + proposed_y_i_val);

    if (y[i] < x[i] && y[i] < max_allowed_for_y_i_to_reach &&
        remaining_needed_for_right_states >= 0 &&
        remaining_needed_for_right_states <= max_right_capacity(i + 1)) {
      y[i] = proposed_y_i_val;

      for (int j = i + 1; j < n_states; ++j) {
        y[j] = 0;
      }

      long long current_sum_after_i_modified =
          std::accumulate(y.begin(), y.end(), 0LL);
      int remaining_to_fill_right = n_alpha - (int)current_sum_after_i_modified;

      for (int j = n_states - 1; j > i && remaining_to_fill_right > 0; --j) {
        int add = std::min(x[j], remaining_to_fill_right);
        y[j] = add;
        remaining_to_fill_right -= add;
      }

      sum_y = std::accumulate(y.begin(), y.end(), 0LL);

      if (sum_y == n_alpha) {
        return true;
      }
    }
  }

  done = true;
  return false;
}

DPLayer::DPLayer(int n_arms, int n_states)
    : n_arms{n_arms}, n_states{n_states} {
  dim = combinations(n_arms + n_states - 1, n_states - 1);
  dp.assign(dim, 0);
}

double &DPLayer::operator[](const DPState &dpstate) {
  return dp[dpstate.relative_idx];
}

int DPLayer::size() const { return dim; }

void swap(DPLayer &a, DPLayer &b) noexcept { a.dp.swap(b.dp); }
