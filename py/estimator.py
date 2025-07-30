import numpy as np

class ArmPullEstimator:
    def __init__(self, N=10, alpha=0.5, N_sim=100_000, initial_state=(60, 60)):
        self.N = N
        self.N_sim = N_sim
        self.alpha = alpha
        self.alpha_int = int(self.N * self.alpha)
        self.initial_state = initial_state
        self.results = []

    def transition_probabilities(self, y):
        if y > self.initial_state[0] or self.alpha_int - y > self.initial_state[1]:
            return float('-inf')
        A = np.random.binomial(n=y, p=0.2, size=self.N_sim)
        B = np.random.binomial(n=self.initial_state[0] - y, p=0.9, size=self.N_sim)
        C = np.random.binomial(n=self.alpha_int - y, p=0.7, size=self.N_sim)
        D = np.random.binomial(n=self.initial_state[1] - (self.alpha_int - y), p=0.25, size=self.N_sim)

        X2 = A + B + C + D
        E_min = np.mean(np.minimum(60, X2))
        return y + E_min

    def estimate(self):
        self.results = []
        for y in range(self.alpha_int + 1):
            value = self.transition_probabilities(y)
            self.results.append((y, value))

        optimal_y, max_value = max(self.results, key=lambda x: x[1])
        return optimal_y, max_value


estimator = ArmPullEstimator(N=120, alpha=0.5, N_sim=100_000, initial_state=(59, 61))
optimal_y, max_value = estimator.estimate()

print(optimal_y, max_value)
