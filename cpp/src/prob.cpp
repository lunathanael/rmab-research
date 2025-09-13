#include "prob.h"

#include <cmath>
#include <numeric>

using namespace std;

struct State::ConstStateIt {
  using iterator_category = std::forward_iterator_tag;
  using value_type = int;
  using difference_type = std::ptrdiff_t;
  using pointer = const int *;
  using reference = const int &;

  const State *state;
  int idx;

  ConstStateIt(const State *s, int i) : state(s), idx(i) {}

  int operator*() const { return (*state)[idx]; }
  ConstStateIt &operator++() {
    ++idx;
    return *this;
  }
  ConstStateIt operator++(int) {
    ConstStateIt tmp = *this;
    ++idx;
    return tmp;
  }
  ConstStateIt operator-(int n) const { return ConstStateIt(state, idx - n); }
  bool operator==(const ConstStateIt &other) const {
    return state == other.state && idx == other.idx;
  }
  bool operator!=(const ConstStateIt &other) const { return !(*this == other); }
};

State::ConstStateIt State::begin() const { return ConstStateIt(this, 0); }
State::ConstStateIt State::end() const { return ConstStateIt(this, n_states); }

State::State(int n_arms, int n_states)
    : val(0), n_arms{n_arms}, n_states{n_states} {}

State::State(const std::vector<int> &v, int n_arms)
    : val(0), n_arms{n_arms}, n_states(v.size()) {
  for (int i = n_states - 1; i >= 0; --i) {
    val = val * (n_arms + 1) + v[i];
  }
}

std::vector<int> State::to_vector() const {
  std::vector<int> v(n_states);
  std::uint64_t x = val;
  for (int i = 0; i < n_states; ++i) {
    v[i] = x % (n_arms + 1);
    x /= (n_arms + 1);
  }
  return v;
}

int State::operator[](int i) const {
  std::uint64_t x = val;
  for (int j = 0; j < i; ++j)
    x /= (n_arms + 1);
  return x % (n_arms + 1);
}

void State::set_at(int idx, int value) {
  std::vector<int> v = to_vector();
  v[idx] = value;
  assign(v);
}

void State::assign(int n, int x) {
  val = 0;
  for (int i = 0; i < n; ++i)
    val = val * (n_arms + 1) + x;
}

void State::assign(const std::vector<int> &v) {
  val = 0;
  for (int i = n_states - 1; i >= 0; --i)
    val = val * (n_arms + 1) + v[i];
}

State State::operator+(const State &other) const {
  State result = *this;
  result.val += other.val;
  return result;
}

bool State::operator<(const State &other) const { return val < other.val; }
bool State::operator==(const State &other) const {
  return val == other.val && n_arms == other.n_arms &&
         n_states == other.n_states;
}

void multinomialDistribution(int n, const vector<double> &probs, ProbDist &out,
                             State current, int idx) {
  int k = probs.size();
  if (idx == k - 1) {
    int _x = n - accumulate(current.begin(), current.end() - 1, 0);
    current.set_at(idx, _x);

    double p = 1.0;
    long long multinomialCoeff = 1;
    long long nFact = 1;
    for (int i = 1; i <= n; ++i)
      nFact *= i;
    multinomialCoeff = nFact;
    for (int i = 0; i < k; ++i) {
      long long xiFact = 1;
      for (int j = 1; j <= current[i]; ++j)
        xiFact *= j;
      multinomialCoeff /= xiFact;

      p *= pow(probs[i], current[i]);
    }

    p *= multinomialCoeff;

    out[current] += p;
    return;
  }
  int remaining = n - accumulate(current.begin(), current.end(), 0);
  for (int i = 0; i <= remaining; ++i) {
    current.set_at(idx, i);
    multinomialDistribution(n, probs, out, current, idx + 1);
  }
}

ProbDist convolve(const ProbDist &a, const ProbDist &b) {
  ProbDist result;
  for (auto &[sa, pa] : a) {
    for (auto &[sb, pb] : b) {
      result[sa + sb] += pa * pb;
    }
  }
  return result;
}

ProbDist fullTransitionDistribution(const vector<int> &S, const vector<int> &A,
                                    const vector<vector<double>> &p0,
                                    const vector<vector<double>> &p1) {
  int k = S.size();
  int n = accumulate(S.begin(), S.end(), 0);
  ProbDist total;
  State initial(n, k);
  initial.assign(k, 0);
  total[initial] = 1.0;
  for (int i = 0; i < k; ++i) {
    ProbDist dist1, dist0, combined;
    multinomialDistribution(A[i], p1[i], dist1, initial);
    multinomialDistribution(S[i] - A[i], p0[i], dist0, initial);
    combined = convolve(dist1, dist0);
    total = convolve(total, combined);
  }
  return total;
}
