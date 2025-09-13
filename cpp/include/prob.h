#pragma once

#include <map>
#include <vector>

using State = std::vector<int>;
using ProbDist = std::map<State, double>;

State addStates(const State &a, const State &b);

void multinomialDistribution(int n, const std::vector<double> &probs,
                             ProbDist &out, State current = State(),
                             int idx = 0);

ProbDist convolve(const ProbDist &a, const ProbDist &b);

ProbDist fullTransitionDistribution(const std::vector<int> &S,
                                    const std::vector<int> &A,
                                    const std::vector<std::vector<double>> &p0,
                                    const std::vector<std::vector<double>> &p1);
