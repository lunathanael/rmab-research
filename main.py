from collections import defaultdict
import itertools

NUMBER_OF_STATES = 2

class State:
    def __init__(self, s: int):
        self.s = s
    
    def __str__(self):
        return f"State(s={self.s})"
    
    def __hash__(self):
        return hash(self.s)
    
    def __eq__(self, other):
        return self.s == other.s
    
    

class RMABState:
    def __init__(self, t: int, s: tuple[float]):
        self.t = t
        self.s = s
    
    def __str__(self):
        return f"RMABState(t={self.t}, s={self.s})"
    
class Action:
    def __init__(self, a: int):
        self.a = a
    
    def __str__(self):
        return f"Action(a={self.a})"
    
    def __repr__(self):
        return str(self)
    
    def __hash__(self):
        return hash(self.a)
    
    def __eq__(self, other):
        return self.a == other.a
    
class RMABAction:
    def __init__(self, a: tuple[int]):
        self.a = a
    
    def __str__(self):
        return f"RMABAction(a={self.a})"
    
    def __repr__(self):
        return str(self)
    
    def __hash__(self):
        return hash(self.a)
    
    def __eq__(self, other):
        return self.a == other.a

class RMAB:
    def __init__(self, H: int, N: int, alpha: float, initial_state: RMABState, rewards: dict[tuple[int, State, Action], float], transition_probabilities: dict[tuple[int, State, State, Action], float]):
        self.H = H
        self.N = N
        self.alpha = alpha
        self.rewards = rewards
        self.transition_probabilities = transition_probabilities
        self.initial_state = initial_state

        self.allowed_actions = int(self.alpha * self.N)
        self.rng = list(range(self.allowed_actions + 1)) * NUMBER_OF_STATES
        self.allowed_actions = set(RMABAction(i) for i in itertools.permutations(self.rng, NUMBER_OF_STATES) if sum(i) == self.allowed_actions)

    def r(self, t: int, s: RMABState, a: RMABAction) -> float:
        reward = 0
        for i in range(NUMBER_OF_STATES):
            reward += self.rewards[(t, State(i), Action(1))] * a.a[i]
            reward += self.rewards[(t, State(i), Action(0))] * (s.s[i] - a.a[i])
        return reward

    def act(self, t: int, s: RMABState, a: RMABAction) -> RMABState:
        next_s = [0] * NUMBER_OF_STATES
        for i in range(NUMBER_OF_STATES):
            for j in range(NUMBER_OF_STATES):
                next_s[i] += self.transition_probabilities[(t, State(i), State(j), Action(1))] * a.a[j]
                next_s[i] += self.transition_probabilities[(t, State(i), State(j), Action(0))] * (s.s[j] - a.a[j])
        return RMABState(t + 1, tuple(next_s))

def find_optimal_policy(rmab: RMAB, s: RMABState) -> tuple[list[Action], float]:
    if s.t > rmab.H:
        return [], 0
    
    best_action = None
    best_value = float('-inf')
    for a in rmab.allowed_actions:
        next_s = rmab.act(s.t, s, a)
        optimal_policy, optimal_value = find_optimal_policy(rmab, next_s)
        value = rmab.r(s.t, s, a) + rmab.alpha * optimal_value
        if value > best_value:
            best_value = value
            best_action = [a] + optimal_policy
    return best_action, best_value

transition_probabilities = {
    (1, State(0), State(0), Action(1)): 0.2,
    (1, State(1), State(0), Action(1)): 0.8,
    (1, State(0), State(0), Action(0)): 0.9,
    (1, State(1), State(0), Action(0)): 0.1,
    (1, State(0), State(1), Action(1)): 0.7,
    (1, State(1), State(1), Action(1)): 0.3,
    (1, State(0), State(1), Action(0)): 0.25,
    (1, State(1), State(1), Action(0)): 0.75,

    # Useless states for H=2
    (2, State(0), State(0), Action(1)): 0.2,
    (2, State(1), State(0), Action(1)): 0.8,
    (2, State(0), State(0), Action(0)): 0.9,
    (2, State(1), State(0), Action(0)): 0.1,
    (2, State(0), State(1), Action(1)): 0.7,
    (2, State(1), State(1), Action(1)): 0.3,
    (2, State(0), State(1), Action(0)): 0.25,
    (2, State(1), State(1), Action(0)): 0.75,
}

rewards = defaultdict(float)
rewards[(1, State(1), Action(1))] = rewards[(2, State(1), Action(1))] = 1

initial_state = RMABState(t=1, s=(1, 1))

rmab = RMAB(H=2, N=2, alpha=0.5, initial_state=initial_state, rewards=rewards, transition_probabilities=transition_probabilities)

optimal_policy, optimal_value = find_optimal_policy(rmab, initial_state)
normalized_value = optimal_value / (rmab.N)
print(optimal_value)
print(optimal_policy)
print(normalized_value)







