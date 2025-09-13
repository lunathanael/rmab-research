import collections

def full_transition_distribution(S: list[int], A: list[int], p0: list[list[float]], p1: list[list[float]]) -> list[tuple[tuple[int, ...], float]]:
    """
    Calculates the exact probabilities of all possible next states given the current system state,
    action assignments, and unit transition probabilities.

    Args:
        S: A list representing the current state, e.g., [s0, s1, s2] means s0 units in state 0, etc.
        A: A list representing actions, e.g., [a0, a1, a2] means a0 units in state 0
           take action 1, while (s0-a0) take action 0.
        p0: A 2D list where p0[i][j] is the probability a unit in state i without action
            moves to state j.
        p1: A 2D list where p1[i][j] is the probability a unit in state i with action
            moves to state j.

    Returns:
        A list of tuples, where each tuple contains (next_state_tuple, probability).
        The next_state_tuple is like (t0, t1, t2).
        The list is sorted by the next_state_tuple.
    """
    n_states = len(S)  # Number of distinct states (e.g., 3 for states 0, 1, 2)
    total_system_units = sum(S) # Total number of units in the system

    # Helper function to calculate the probability distribution of final states
    # for a given number of units starting from a specific initial state,
    # applying a particular transition matrix (p0 or p1).
    def get_sub_distribution(num_units: int, transition_probs_one_unit: list[float]) -> dict[tuple[int, ...], float]:
        """
        Computes the probability distribution for 'num_units' when each unit transitions
        independently according to 'transition_probs_one_unit'.

        Args:
            num_units: The number of units to consider.
            transition_probs_one_unit: A list of probabilities [p_to_state0, p_to_state1, ...].

        Returns:
            A dictionary where keys are tuples representing final state counts (e.g., (t0, t1, t2))
            and values are their corresponding probabilities.
        """
        # dp stores: { (final_count_state0, final_count_state1, ...): probability }
        # Initial state: 0 units have transitioned, resulting in 0 units in each state, with probability 1.0.
        dp = {tuple([0] * n_states): 1.0}

        # For each unit to be processed
        for _ in range(num_units):
            new_dp = collections.defaultdict(float)
            # Iterate through existing distributions from previous units
            for current_counts, current_prob in dp.items():
                # For each possible target state a single unit can move to
                for target_state_idx in range(n_states):
                    prob_this_unit_to_target = transition_probs_one_unit[target_state_idx]
                    
                    # Create the next state tuple by incrementing the count for target_state_idx
                    next_counts = list(current_counts)
                    next_counts[target_state_idx] += 1
                    next_counts_tuple = tuple(next_counts)
                    
                    # Add this path's probability to the new distribution
                    new_dp[next_counts_tuple] += current_prob * prob_this_unit_to_target
            dp = new_dp # Update DP table for the next unit
        return dp

    # Helper function to combine two probability distributions (perform a convolution).
    # This is used to merge distributions from different groups of units.
    def combine_distributions(dist1: dict[tuple[int, ...], float], dist2: dict[tuple[int, ...], float]) -> dict[tuple[int, ...], float]:
        """
        Combines two probability distributions into a single one.
        If dist1 = {(s1_counts): p1} and dist2 = {(s2_counts): p2},
        the combined distribution will have {(s1_counts + s2_counts): p1*p2}.
        """
        combined_dist = collections.defaultdict(float)
        for counts1, prob1 in dist1.items():
            for counts2, prob2 in dist2.items():
                # Sum the counts for each state and multiply probabilities
                new_counts = tuple(c1 + c2 for c1, c2 in zip(counts1, counts2))
                combined_dist[new_counts] += prob1 * prob2
        return combined_dist

    # Initialize the final distribution. Start with an empty system having a probability of 1.0.
    # As we process groups of units, their distributions will be combined into this.
    final_distribution = {tuple([0] * n_states): 1.0}

    # Iterate through each initial state (e.g., state 0, state 1, state 2)
    for initial_state_idx in range(n_states):
        # Calculate units that do NOT take action 1
        num_no_action = S[initial_state_idx] - A[initial_state_idx]
        if num_no_action > 0:
            # Get distribution for these units using p0 probabilities
            dist_no_action = get_sub_distribution(num_no_action, p0[initial_state_idx])
            # Combine it with the accumulating final distribution
            final_distribution = combine_distributions(final_distribution, dist_no_action)

        # Calculate units that DO take action 1
        num_with_action = A[initial_state_idx]
        if num_with_action > 0:
            # Get distribution for these units using p1 probabilities
            dist_with_action = get_sub_distribution(num_with_action, p1[initial_state_idx])
            # Combine it with the accumulating final distribution
            final_distribution = combine_distributions(final_distribution, dist_with_action)

    # Filter out states whose total unit count does not match total_system_units.
    # (This step is mostly for robustness; the DP construction should naturally yield this sum for all valid states).
    filtered_distribution = {
        state: prob for state, prob in final_distribution.items()
        if sum(state) == total_system_units
    }
    
    # Sort the results by the state tuple for consistent output.
    sorted_results = sorted(filtered_distribution.items())

    return sorted_results

def generate_actions(state):
    actions = []
    for i in range(5+1):
        other = 5 - i
        if i > state[0] or other > state[1]:
            continue
        actions.append((i, other))
    return actions
    
def calc(state, action, r):
    reward = 0
    for i in range(2):
        reward += r[i][1] * action[i] + r[i][0] * (state[i] - action[i])
    return reward

# Given data as per the problem description
S = [0, 10] # Current state: 2 units in state 0, 1 in state 1, 2 in state 2
A = [0, 5] # Action: 1 unit in state 0 takes action 1, 0 in state 1, 2 in state 2
# p0[i][j]: prob a unit in state i without action moves to state j
p0 = [[0.9, 0.1],
      [0.25, 0.75]]
# p1[i][j]: prob a unit in state i with action moves to state j
p1 = [[0.2, 0.8],
      [0.7, 0.3]]
      
r = [[0, 1],
     [0, 0]]

# Calculate the full transition distribution
result = full_transition_distribution(S, A, p0, p1)

states = [a[0] for a in result]

curr = {}; prev = collections.defaultdict(float)

for i in range(1, -1, -1):
    for state in states:
        mxr = 0
        for action in generate_actions(state):
            reward = calc(state, action, r)
            q = full_transition_distribution(state, action, p0, p1)
            q = dict(q)
            for s2 in states:
                reward += q[s2] * prev[s2]
            mxr = max(mxr, reward)
        curr[state] = mxr
    curr, prev = prev, curr
print(prev)