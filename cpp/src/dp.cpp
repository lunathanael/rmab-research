#include "dp.h"
#include "utils.h"
#include <algorithm>
#include <numeric>

using namespace std;

DPStateIterator::DPStateIterator(int n_arms, int n_states)
    : n{combinations(n_arms + n_states - 1, n_states - 1)}, n_arms(n_arms),
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

int DPStateIterator::size() const { return n; }

long long StateActionIterator::max_right_capacity(int start_idx) const {
  long long capacity = 0;
  for (int k = start_idx; k < n_states; ++k) {
    capacity += x[k];
  }
  return capacity;
}

StateActionIterator::StateActionIterator(const DPStateIterator &it, int n_alpha)
    : x(it.current()), n_states(it.current().size()), n_alpha(n_alpha),
      done(false), started{false} {
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

double &DPLayer::operator[](const DPStateIterator &dpstate) {
  return dp[dpstate.idx];
}

void swap(DPLayer &a, DPLayer &b) noexcept { a.dp.swap(b.dp); }
