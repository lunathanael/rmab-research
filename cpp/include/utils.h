#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

constexpr int combinations(int n, int r) {
  if (r > n || r < 0 || n < 0)
    return 0;
  if (r == 0 || r == n)
    return 1;

  long long result = 1;
  r = std::min(r, n - r);

  for (int i = 0; i < r; ++i) {
    result = result * (n - i) / (i + 1);
  }
  return static_cast<int>(result);
}

constexpr unsigned long long combinationsull(int n, int k) {
  if (k < 0 || k > n)
    return 0ULL;
  if (k == 0 || k == n)
    return 1ULL;
  if (k > n - k)
    k = n - k;
  uint64_t res = 1;
  for (int i = 1; i <= k; ++i) {
    res = res * (n - k + i);
    res /= i;
  }
  return static_cast<unsigned long long>(res);
}

class BitArray {
  static constexpr int MASK_BITS = 8;
  static constexpr std::uint64_t MASK = (1ULL << MASK_BITS) - 1;
  std::uint64_t val;

public:
  BitArray() : val{0} {}
  BitArray(const std::vector<int> &v);

  std::vector<int> to_vector() const;
  int operator[](int idx) const;
  void set_at(int idx, int x);
  int sum() const;

  operator std::uint64_t() const { return val; }
};
