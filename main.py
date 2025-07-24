from collections import defaultdict
import itertools

import utils as ut


class RMAB:
    def __init__(self, H: int, N: int, alpha: float, initial_state: tuple[int], rewards: dict[tuple[int, int, int], float], transition_probabilities: dict[tuple[int, int, int, int], float]):
        self.H = H
        self.N = N
        self.alpha = alpha
        self.rewards = rewards
        self.transition_probabilities = transition_probabilities
        self.initial_state = initial_state

        self.number_of_states = len(initial_state)
        rng = list(range(self.N + 1)) * self.number_of_states
        self.all_states = set(x for x in itertools.permutations(rng, self.number_of_states) if sum(x) == self.N)

        self.transition_array = [[[[[0 for l in range(self.number_of_states)] for k in range(self.number_of_states)] for j in range(2)] for i in range(self.number_of_states)] for _ in range(self.H)]
        for t in range(self.H):
            for s in range(self.number_of_states):
                for i in range(self.number_of_states):
                    for a in range(2):
                        self.transition_array[t][a][s][i] = self.transition_probabilities[(t + 1, i, s, a)]


    def r(self, t: int, s: tuple[int], a: tuple[int]) -> float:
        reward = 0
        for i in range(self.number_of_states):
            reward += self.rewards[(t, i, 1)] * a[i]
            reward += self.rewards[(t, i, 0)] * (s[i] - a[i])
        return reward
    
    def generate_actions(self, s: tuple[int]) -> set[tuple[int]]:
        actions = [(0,tuple())]
        pulls = int(self.alpha * self.N)
        for i in range(self.number_of_states - 1):
            tmp = []
            for j in range(min(s[i], pulls) + 1):
                for k in actions:
                    if k[0] + j <= pulls:
                        tmp.append((k[0] + j, k[1] + (j,)))
            actions = tmp
        actions = set(x[1] + (pulls - x[0],) for x in actions if pulls - x[0] <= s[-1])
        return actions

#     def act(self, t: int, s: tuple[int], a: tuple[int]) -> tuple[int]:
#         next_s = [0] * self.number_of_states
#         for i in range(self.number_of_states):
#             for j in range(self.number_of_states):
#                 next_s[i] += self.transition_probabilities[(t, State(i), State(j), Action(1))] * a.a[j]
#                 next_s[i] += self.transition_probabilities[(t, State(i), State(j), Action(0))] * (s.s[j] - a.a[j])
#         return RMABState(t + 1, tuple(next_s))

def find_optimal_policy(rmab: RMAB):
    rewards = defaultdict(lambda: (tuple(), float('-inf')))
    for t in range(rmab.H, 0, -1):
        for s in rmab.all_states:
            for a in rmab.generate_actions(s):
                reward = rmab.r(t, s, a)
                op_a = tuple(map(lambda x, y: x - y, s, a))
                probs = ut.get_action_probs2(a, rmab.transition_array[t - 1][1], op_a, rmab.transition_array[t - 1][0])
                for i in rmab.all_states:
                    if t != rmab.H:
                        reward += rewards[(t+1, i)][1] * probs[i]
                if reward > rewards[(t, s)][1]:
                    rewards[(t, s)] = (a, reward)
    return rewards[(1, rmab.initial_state)][1], rewards

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

initial_state = (1,1)

rmab = RMAB(H=2, N=2, alpha=0.5, initial_state=initial_state, rewards=rewards, transition_probabilities=transition_probabilities)

optimal_value, _ = find_optimal_policy(rmab)
print(_)
normalized_value = optimal_value / (rmab.N)
print(optimal_value)
print(normalized_value)







