This project provides a C++ implementation for computing explicit optimal solutions to finite-horizon Restless Multi-Armed Bandit (RMAB) problems. The software leverages dynamic programming to move beyond heuristic estimates and find precise solutions.

## Background

The Restless Multi-Armed Bandit (RMAB) problem is a model used for optimizing resource allocation under uncertainty, where a decision-maker must choose which subset of projects or "arms" to activate at each time step to maximize a cumulative reward. This project addresses scenarios where traditional approximation policies may be suboptimal. By computing the explicit optimal solution, it provides a baseline for evaluating the performance of heuristic methods.

## Techniques Utilized

*   **Dynamic Programming:** The core of the solution is a dynamic programming algorithm that solves the problem by iterating backward in time. It computes the optimal value function for each possible state at each time step, ensuring a globally optimal solution.

*   **Parallelization:** To handle the computational complexity of the dynamic programming approach, the algorithm is parallelized using OpenMP. This allows for the simultaneous evaluation of states within each layer of the dynamic program, significantly reducing computation time from minutes to seconds.

*   **Memory Optimization:** The state space of the problem can be very large. To manage memory usage efficiently, states are represented using vectorized integer bitarrays. This custom data structure provides a compact in-memory representation of the system's state, achieving an 8x reduction in memory usage while maintaining constant-time access to state information. Profiling with gprof was used to identify and resolve data bottlenecks in performance-critical loops.


<img width="1865" height="1315" alt="image" src="https://github.com/user-attachments/assets/168c3d7c-43ec-4634-9c5d-f098b4946871" />
<img width="1773" height="1338" alt="image" src="https://github.com/user-attachments/assets/56b367d9-520f-4bde-a184-ca11130be9db" />
<img width="3653" height="2044" alt="image" src="https://github.com/user-attachments/assets/2915a414-4a7b-447a-90f9-4e679301f15e" />
<img width="2966" height="1557" alt="image" src="https://github.com/user-attachments/assets/e676138c-4807-4ff7-b37a-48d7395ed389" />
<img width="1338" height="330" alt="image" src="https://github.com/user-attachments/assets/e5b53411-a9f7-40e9-b733-718c9a90c8d9" />

### Defining Optimality: Maximizing Total Expected Reward

The primary goal is to find a policy that maximizes the total expected reward over a finite time horizon. An optimal action at any given time does not just consider the immediate reward, but also how that action influences future states and, consequently, future rewards.

This is achieved using the principle of dynamic programming and the Bellman equation. The value of being in a particular state `S` at time `t`, denoted `V(S, t)`, is defined as the maximum possible expected reward from time `t` until the end of the horizon. The value is calculated by choosing the action `A` that maximizes the sum of two components:

1.  **Immediate Reward:** The deterministic reward gained from taking action `A` in state `S` at time `t`.
2.  **Expected Future Reward:** The weighted average of the values of all possible subsequent states `S'`. Each future state's value `V(S', t+1)` is weighted by the probability of transitioning from `S` to `S'` when action `A` is taken.

The term "explicit full expected value" signifies that the algorithm computes this value exactly. Unlike approximation methods that might use sampling or heuristics, this implementation calculates the precise probability for every possible next state and uses these to compute the true expected value. This guarantees optimality but comes at a high computational cost, necessitating the optimizations below.

---

### Technique 1: Combinatorial Indexing for State Mapping

A significant challenge in dynamic programming is managing the state space. A state is represented by a vector of integers `(x_1, x_2, ..., x_k)` where the sum is `n_arms`. To store the value for each state in a simple array, we need a way to uniquely map each state vector to an integer index.

This is accomplished using combinatorial indexing. The total number of unique states corresponds to the number of ways to place `n_arms` identical items into `n_states` distinct bins, a classic combinatorics problem. The `DPState` class calculates a unique `relative_idx` for each state vector. This index is its position in a lexicographically sorted list of all possible state vectors. The calculation works by iterating through the state vector and summing up the number of valid combinations that would have appeared before the current one. This mapping allows for `O(1)` lookup of any state's value in the dynamic programming table, which is crucial for performance.

### Technique 2: Exact Transition Probability Calculation

To compute the expected future reward, the exact probability distribution over all possible next states must be determined. This calculation is handled by the `MultiDist` class.

The process for a given state `S` and action `A` is as follows:

1.  **Partition Arms:** For each state `i`, the `S[i]` arms are partitioned into two groups: `A[i]` arms that are activated (and transition according to probabilities `p1`) and `S[i] - A[i]` arms that remain passive (and transition according to `p0`).
2.  **Multinomial Distributions:** The outcomes for a group of arms in the same state undergoing the same action follow a multinomial distribution. For example, the `A[i]` active arms will transition to new states according to the probabilities in `p1[i]`. The `multinomialDistribution` function calculates the probability for every possible resulting distribution of these arms.
3.  **Convolution:** The transitions of arms starting in different states are independent events. To find the probability distribution for the entire system, the individual distributions must be combined. This is achieved through convolution. The `convolve` function takes the probability distributions for two independent groups of arms and computes the combined distribution of their sum.
4.  **Full Distribution:** The `fullTransitionDistribution` function orchestrates this process. It iterates through each initial state `i`, generates the two multinomial distributions for the active and passive arms, convolves them together, and then convolves this result with the aggregate distribution computed so far. The final result is a complete probability map from every possible next state to its exact probability of occurring.

### Technique 3: Parallelized Computation with OpenMP

The dynamic programming approach is computationally intensive. To make it feasible, the main loop that evaluates all states within a given time step is parallelized using OpenMP.

The `#pragma omp parallel for schedule(dynamic)` directive instructs the compiler to distribute the iterations of the loop across multiple CPU cores. This allows for the simultaneous evaluation of different states, dramatically reducing the overall computation time from over 20 minutes to just a few seconds on multi-core hardware.

### Technique 4: Memory-Efficient State Representation

To handle the large number of states, a custom `BitArray` class provides a memory-efficient representation. Instead of a standard `std::vector<int>`, it packs multiple small integer counts into a single 64-bit unsigned integer using bitwise shifts and masks. This reduces the memory footprint for storing states by a factor of 8, enabling the program to solve problems with much larger state spaces. The code was profiled with `gprof` to identify and resolve data bottlenecks, ensuring this compact representation did not compromise access speed.
