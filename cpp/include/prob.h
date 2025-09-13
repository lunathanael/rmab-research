#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include <unordered_map>
#include <ext/pb_ds/assoc_container.hpp>

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

  operator std::uint64_t() const { return val; }
};
struct chash {
	// any random-ish large odd number will do
	static constexpr uint64_t C = uint64_t(2e18 * M_PI) + 71;
	// random 32-bit number
	size_t operator()(std::uint64_t x) const {
		// see https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
		return __builtin_bswap64(x * C);
	}
};
using ProbDist = __gnu_pbds::gp_hash_table<std::uint64_t, double, chash>;

void multinomialDistribution(int n, const std::vector<double> &probs,
                             ProbDist &out, State current, int idx = 0);

ProbDist convolve(const ProbDist &a, const ProbDist &b);

ProbDist fullTransitionDistribution(const std::vector<int> &S,
                                    const std::vector<int> &A,
                                    const std::vector<std::vector<double>> &p0,
                                    const std::vector<std::vector<double>> &p1);
