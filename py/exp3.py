from main import RMAB, find_optimal_policy
from collections import defaultdict


# New experiment with 10x10 transition matrices and reward vectors
transition_probabilities_new = defaultdict(float)
transition_probabilities_new.update({
    (1, 0, 0, 0): 0.70317263, (1, 1, 0, 0): 0.29682737,
    (1, 1, 1, 0): 0.03159578, (1, 3, 1, 0): 0.96840422,
    (1, 2, 2, 0): 0.37932461, (1, 3, 2, 0): 0.62067539,
    (1, 0, 3, 0): 0.30200956, (1, 3, 3, 0): 0.69799044,
    
    (1, 2, 0, 1): 0.08423799, (1, 3, 0, 1): 0.91576201,
    (1, 0, 1, 1): 0.80689287, (1, 2, 1, 1): 0.19310713,
    (1, 0, 2, 1): 0.20350136, (1, 1, 2, 1): 0.79649864,
    (1, 1, 3, 1): 0.16178167, (1, 3, 3, 1): 0.83821833,
})


for h in range(2, 3 + 1):
    for i in range(4):
        for j in range(4):
            transition_probabilities_new[(h, i, j, 0)] = transition_probabilities_new[(1, i, j, 0)]
            transition_probabilities_new[(h, i, j, 1)] = transition_probabilities_new[(1, i, j, 1)]


# New rewards based on r(:,0) and r(:,1) vectors
rewards_new = defaultdict(float)

# Action 0 rewards (r(:,0))
rewards_new[(1, 0, 0)] = 0.44926316
rewards_new[(1, 1, 0)] = 0.21953011
rewards_new[(1, 2, 0)] = 0.10236761
rewards_new[(1, 3, 0)] = 0.27516813
# Action 1 rewards (r(:,1))
rewards_new[(1, 0, 1)] = 0.10939499
rewards_new[(1, 1, 1)] = 0.85231770
rewards_new[(1, 2, 1)] = 0.06171567
rewards_new[(1, 3, 1)] = 0.97393949

for h in range(2, 3 + 1):
    for i in range(4):
        for j in range(4):
            rewards_new[(h, i, j)] = rewards_new[(1, i, j)]

# New initial state based on x_ini vector: [0, 0.5, 0, 0, 0, 0, 0, 0.5, 0, 0]
# For a 10-state system, we need to scale this to work with N=120
# Let's use a 10-state system with N=120, so each state can have up to 120
initial_state_new = (0, 25, 0, 25)  # Scaled version of x_ini


# Run new experiment
print("\n=== New Experiment (10-state system) ===")
rmab_new = RMAB(H=3, N=50, alpha=0.5, initial_state=initial_state_new, rewards=rewards_new, transition_probabilities=transition_probabilities_new)

optimal_value_new, _ = find_optimal_policy(rmab_new)
print(_)
normalized_value_new = optimal_value_new / (rmab_new.N)
print(optimal_value_new)
print(normalized_value_new)







