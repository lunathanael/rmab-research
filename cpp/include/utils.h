#pragma once

#include <algorithm>

constexpr int combinations(int n, int r) {
    if (r > n || r < 0 || n < 0) return 0;
    if (r == 0 || r == n) return 1;
    
    long long result = 1;
    r = std::min(r, n - r);
    
    for (int i = 0; i < r; ++i) {
        result = result * (n - i) / (i + 1);
    }
    return static_cast<int>(result);
}