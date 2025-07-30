from collections import defaultdict
from functools import lru_cache
import math
import tqdm
import time
import random

import utils as ut


class RMAB:
    def __init__(
        self,
        H: int,
        N: int,
        alpha: float,
        initial_state: tuple[int],
        rewards: dict[tuple[int, int, int], float],
        transition_probabilities: dict[tuple[int, int, int, int], float],
    ):
        self.H = H
        self.N = N
        self.alpha = alpha
        self.rewards = rewards
        self.transition_probabilities = transition_probabilities
        self.initial_state = initial_state

        self.number_of_states = len(initial_state)
        self.all_states = ut.stars_and_bars(self.N, self.number_of_states)

        self.transition_array = [
            [
                [
                    [
                        [0 for m in range(self.number_of_states)]
                        for k in range(self.number_of_states)
                    ]
                    for j in range(self.number_of_states)
                ]
                for i in range(2)
            ]
            for _ in range(self.H)
        ]

        for t in range(self.H):
            for s in range(self.number_of_states):
                for i in range(self.number_of_states):
                    for a in range(2):
                        self.transition_array[t][a][s][i] = (
                            self.transition_probabilities[(t + 1, i, s, a)]
                        )

        self.transition_array = tuple(
            tuple(tuple(tuple(x) for x in y) for y in z) for z in self.transition_array
        )

    def r(self, t: int, s: tuple[int], a: tuple[int]) -> float:
        reward = 0
        for i in range(self.number_of_states):
            reward += self.rewards[(t, i, 1)] * a[i]
            reward += self.rewards[(t, i, 0)] * (s[i] - a[i])
        return reward

    @lru_cache(maxsize=None)
    def generate_actions(self, s: tuple[int]) -> set[tuple[int]]:
        actions = [(0, tuple())]
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


gloops_per_sec = 1

def find_optimal_policy(rmab: RMAB, verbose=True):
    rewards = defaultdict(lambda: (tuple(), float("-inf")))

    # Calculate total work for better progress tracking
    total_states = len(rmab.all_states)
    total_work = rmab.H * total_states

    if verbose:
        print(f"\nStarting optimal policy computation:")
        print(f"   Time horizon: {rmab.H} periods")
        print(f"   Total states: {total_states:,}")
        print(f"   Total work: {total_work:,} state-period combinations")
        print(
            f"   Alpha: {rmab.alpha} (max pulls per period: " f"{int(rmab.alpha * rmab.N)})"
        )
        operations = rmab.H * rmab.N ** (4 * (rmab.number_of_states - 1))
        print(
            f"operating at complexity O(H * N^(4 * (num_states - 1))), for H={rmab.H}, N={rmab.N}, num_states={rmab.number_of_states} this is approximately 10^{math.log10(operations):.1f} or 2^{math.log2(operations):.1f}"
        )
        secs = int(operations / ops_per_sec)
        print(f"Estimated time: {secs//86400}d {(secs%86400)//3600}h {(secs%3600)//60}m {secs%60}s")

    with tqdm.tqdm(
        total=total_work,
        desc="Overall Progress",
        unit="state-period",
        bar_format="{l_bar}{bar}| {n_fmt}/{total_fmt} "
        "[{elapsed}<{remaining}, {rate_fmt}]",
        disable=not verbose,
    ) as pbar:

        for t in range(rmab.H, 0, -1):
            period_desc = f"⏰ Period {t}/{rmab.H}"
            with tqdm.tqdm(
                rmab.all_states,
                desc=period_desc,
                leave=False,
                unit="states",
                bar_format="{l_bar}{bar}| {n_fmt}/{total_fmt} "
                "[{elapsed}<{remaining}]",
                disable=not verbose,
            ) as state_pbar:

                for s in state_pbar:
                    state_info = f"State {s} (sum={sum(s)})"
                    state_pbar.set_description(f"{period_desc} - {state_info}")

                    for a in rmab.generate_actions(s):
                        reward = rmab.r(t, s, a)
                        op_a = tuple(map(lambda x, y: x - y, s, a))
                        probs = ut.get_action_probs2(
                            a,
                            rmab.transition_array[t - 1][1],
                            op_a,
                            rmab.transition_array[t - 1][0],
                        )
                        for i in rmab.all_states:
                            if t != rmab.H:
                                reward += rewards[(t + 1, i)][1] * probs[i]
                        if reward > rewards[(t, s)][1]:
                            rewards[(t, s)] = (a, reward)

                    pbar.update(1)

    if verbose:
        print(f"\nOptimal policy computation completed!")
        print(f"   Final optimal value: " f"{rewards[(1, rmab.initial_state)][1]:.4f}")
        print(
            f"   Normalized value: " f"{rewards[(1, rmab.initial_state)][1] / rmab.N:.4f}"
        )

    return rewards[(1, rmab.initial_state)][1], rewards


def run_mini_benchmark(H=3, N=15, alpha=0.4, num_states=3, initial_state=(3, 8, 4)):
    global ops_per_sec
    assert num_states == len(initial_state)
    print("\nStarting Mini Benchmark (~6 seconds)...")

    transition_probabilities = {}
    rewards = defaultdict(float)

    for t in range(1, H + 1):
        for s in range(num_states):
            for a in range(2):
                raw_probs = [random.random() for _ in range(num_states)]
                total = sum(raw_probs)
                for i in range(num_states):
                    transition_probabilities[(t, i, s, a)] = raw_probs[i] / total
                
                if a == 1:
                    rewards[(t, s, a)] = random.uniform(-2, 2)
                else:
                    rewards[(t, s, a)] = random.uniform(-10, 0)

    rmab = RMAB(H, N, alpha, initial_state, rewards, transition_probabilities)

    start_time = time.time()

    find_optimal_policy(rmab, verbose=False)
    total_ops = H * N ** (4 * (num_states - 1))

    elapsed = time.time() - start_time

    print(f"\nMini Benchmark Results ({elapsed:.2f}s):")
    print("   Operations per second:")

    print(f"   Total operations: {total_ops:,}")
    print(f"   Average ops/sec: {total_ops/elapsed:,.0f}")
    
    ops_per_sec = total_ops / elapsed

run_mini_benchmark()