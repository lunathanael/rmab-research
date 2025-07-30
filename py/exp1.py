from main import RMAB, find_optimal_policy
from collections import defaultdict

transition_probabilities = {
    (1, 0, 0, 1): 0.2,
    (1, 1, 0, 1): 0.8,
    (1, 0, 0, 0): 0.9,
    (1, 1, 0, 0): 0.1,
    (1, 0, 1, 1): 0.7,
    (1, 1, 1, 1): 0.3,
    (1, 0, 1, 0): 0.25,
    (1, 1, 1, 0): 0.75,

    # Useless states for H=2
    (2, 0, 0, 1): 0.2,
    (2, 1, 0, 1): 0.8,
    (2, 0, 0, 0): 0.9,
    (2, 1, 0, 0): 0.1,
    (2, 0, 1, 1): 0.7,
    (2, 1, 1, 1): 0.3,
    (2, 0, 1, 0): 0.25,
    (2, 1, 1, 0): 0.75,
}

rewards = defaultdict(float)
rewards[(1, 0, 1)] = rewards[(2, 0, 1)] = 1

initial_state = (60,60)

rmab = RMAB(H=2, N=120, alpha=0.5, initial_state=initial_state, rewards=rewards, transition_probabilities=transition_probabilities)

optimal_value, _ = find_optimal_policy(rmab)
# print(_)
normalized_value = optimal_value / (rmab.N)
print(optimal_value)
print(normalized_value)







