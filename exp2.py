from main import RMAB, find_optimal_policy
from collections import defaultdict


# New experiment with 10x10 transition matrices and reward vectors
transition_probabilities_new = defaultdict(float)
transition_probabilities_new.update({
    # P(· | ·, 0) matrix - first 5 rows (states 0-4)
    (1, 0, 0, 0): 0.5415, (1, 1, 0, 0): 0.4585,
    (1, 0, 1, 0): 0.5471, (1, 1, 1, 0): 0.2265, (1, 2, 1, 0): 0.2265,
    (1, 0, 2, 0): 0.7067, (1, 2, 2, 0): 0.1467, (1, 3, 2, 0): 0.1467,
    (1, 0, 3, 0): 0.8578, (1, 3, 3, 0): 0.0711, (1, 4, 3, 0): 0.0711,
    (1, 0, 4, 0): 0.9214, (1, 4, 4, 0): 0.0786,
    
    # P(· | ·, 0) matrix - last 5 rows (states 5-9)
    (1, 5, 5, 0): 0.6396, (1, 6, 5, 0): 0.3604,
    (1, 5, 6, 0): 0.5694, (1, 6, 6, 0): 0.2153, (1, 7, 6, 0): 0.2153,
    (1, 5, 7, 0): 0.6453, (1, 7, 7, 0): 0.1773, (1, 8, 7, 0): 0.1773,
    (1, 5, 8, 0): 0.7007, (1, 8, 8, 0): 0.1496, (1, 9, 8, 0): 0.1496,
    (1, 5, 9, 0): 0.7097, (1, 9, 9, 0): 0.2903,
    
    # P(· | ·, 1) matrix - first 5 rows (states 0-4)
    (1, 0, 0, 1): 1.0,
    (1, 0, 1, 1): 0.7337, (1, 1, 1, 1): 0.2663,
    (1, 0, 2, 1): 0.7265, (1, 2, 2, 1): 0.2735,
    (1, 0, 3, 1): 0.6146, (1, 3, 3, 1): 0.3854,
    (1, 0, 4, 1): 0.6054, (1, 4, 4, 1): 0.3946,
    
    # P(· | ·, 1) matrix - last 5 rows (states 5-9)
    (1, 5, 5, 1): 1.0,
    (1, 5, 6, 1): 0.6037, (1, 6, 6, 1): 0.3963,
    (1, 5, 7, 1): 0.6004, (1, 7, 7, 1): 0.3996,
    (1, 5, 8, 1): 0.7263, (1, 8, 8, 1): 0.2737,
    (1, 5, 9, 1): 0.6138, (1, 9, 9, 1): 0.3862,
})



for h in range(2, 6):
    for i in range(10):
        for j in range(10):
            transition_probabilities_new[(h, i, j, 0)] = transition_probabilities_new[(1, i, j, 0)]
            transition_probabilities_new[(h, i, j, 1)] = transition_probabilities_new[(1, i, j, 1)]


# New rewards based on r(:,0) and r(:,1) vectors
rewards_new = defaultdict(float)
# r(:,0) vector: [0, -5.4707, -7.0669, -8.5784, -9.2141, 0, -5.6942, -6.4534, -7.0074, -7.097]
# r(:,1) vector: [-1.9963, -2.085, -2.035, -2.0661, -1.9581, -1.994, -1.9647, -2.2478, -2.0468, -2.2821]

# Action 0 rewards (r(:,0))
rewards_new[(1, 0, 0)] = 0.0
rewards_new[(1, 1, 0)] = -5.4707
rewards_new[(1, 2, 0)] = -7.0669
rewards_new[(1, 3, 0)] = -8.5784
rewards_new[(1, 4, 0)] = -9.2141
rewards_new[(1, 5, 0)] = 0.0
rewards_new[(1, 6, 0)] = -5.6942
rewards_new[(1, 7, 0)] = -6.4534
rewards_new[(1, 8, 0)] = -7.0074
rewards_new[(1, 9, 0)] = -7.097

# Action 1 rewards (r(:,1))
rewards_new[(2, 0, 1)] = -1.9963
rewards_new[(2, 1, 1)] = -2.085
rewards_new[(2, 2, 1)] = -2.035
rewards_new[(2, 3, 1)] = -2.0661
rewards_new[(2, 4, 1)] = -1.9581
rewards_new[(2, 5, 1)] = -1.994
rewards_new[(2, 6, 1)] = -1.9647
rewards_new[(2, 7, 1)] = -2.2478
rewards_new[(2, 8, 1)] = -2.0468
rewards_new[(2, 9, 1)] = -2.2821

# New initial state based on x_ini vector: [0, 0.5, 0, 0, 0, 0, 0, 0.5, 0, 0]
# For a 10-state system, we need to scale this to work with N=120
# Let's use a 10-state system with N=120, so each state can have up to 120
initial_state_new = (0, 5, 0, 0, 0, 0, 5, 0, 0, 0)  # Scaled version of x_ini


# Run new experiment
print("\n=== New Experiment (10-state system) ===")
rmab_new = RMAB(H=5, N=10, alpha=0.4, initial_state=initial_state_new, rewards=rewards_new, transition_probabilities=transition_probabilities_new)

optimal_value_new, _ = find_optimal_policy(rmab_new)
print(_)
normalized_value_new = optimal_value_new / (rmab_new.N)
print(optimal_value_new)
print(normalized_value_new)







