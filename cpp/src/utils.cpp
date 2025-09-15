#include "utils.h"

using namespace std;

BitArray::BitArray(const std::vector<int> &v) : val{0} {
  for (int i = 0; i < v.size(); ++i) {
    set_at(i, v[i]);
  }
}

std::vector<int> BitArray::to_vector() const {
  std::vector<int> v;
  for (int i = 0; i * MASK_BITS < sizeof(val) * 8; ++i) {
    v.emplace_back(this->operator[](i));
  }
  return v;
}

int BitArray::operator[](int idx) const {
  return (val >> (idx * MASK_BITS)) & MASK;
}
void BitArray::set_at(int idx, int x) {
  val = (val & ~(MASK << (idx * MASK_BITS))) | (x << (idx * MASK_BITS));
}
int BitArray::sum() const {
  int total = 0;
  for (auto x = val; x; x >>= MASK_BITS) {
    total += x & MASK;
  }
  return total;
}