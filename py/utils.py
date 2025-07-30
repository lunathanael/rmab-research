import math
import itertools
from collections import defaultdict
import matplotlib.pyplot as plt
from functools import lru_cache

@lru_cache(maxsize=None)
def combinations(n: int, r: int) -> int:
    ans = 1
    for i in range(n, n-r, -1):
        ans *= i
    for i in range(1, r+1):
        ans //= i
    return ans

combinations = math.comb

@lru_cache(maxsize=None)
def check_probs(probs: tuple[tuple[float]], n: int) -> bool:
    atol = 1e-4
    if len(probs) != n:
        return False
    for i in probs:
        if len(i) != n:
            return False
        if abs(sum(i) - 1) > atol:
            return False
    return True

@lru_cache(maxsize=None)
def multinomial(x: int, goal: tuple[int], probs: tuple[float]) -> float:
    n = len(goal)
    assert len(probs) == n
    assert sum(goal) == x
    ans = math.factorial(x)
    for i in range(n):
        ans //= math.factorial(goal[i])
    for i in range(n):
        ans *= probs[i] ** goal[i]
    return ans

def get_action_probs(initial_counts: tuple[int], probs: tuple[tuple[float]]) -> tuple[tuple[float]]:
    n = len(initial_counts)
    assert check_probs(probs, n)

    prev = {tuple([0] * n): 1}
    
    for i in range(n):
        temp = defaultdict(float)
        rng = list(range(initial_counts[i] + 1)) * n
        allocations = set(x for x in itertools.permutations(rng, n) if sum(x) == initial_counts[i])
        for key, value in prev.items():
            for allocation in allocations:
                temp[tuple(map(lambda x, y: x + y, key, allocation))] += value * multinomial(initial_counts[i], allocation, probs[i])
        prev = temp
    return prev

@lru_cache(maxsize=None)
def get_action_probs2(initial_counts0: tuple[int], probs0: tuple[tuple[float]], initial_counts1: tuple[int], probs1: tuple[tuple[float]]) -> tuple[tuple[float]]:
    n = len(initial_counts0)
    assert n == len(initial_counts1)
    assert check_probs(probs0, n)
    assert check_probs(probs1, n)

    prev = {tuple([0] * n): 1}
    
    for i in range(n):
        temp = defaultdict(float)
        allocations = stars_and_bars(initial_counts0[i], n)
        for key, value in prev.items():
            for allocation in allocations:
                temp[tuple(map(lambda x, y: x + y, key, allocation))] += value * multinomial(initial_counts0[i], allocation, probs0[i])
        prev = temp
    
    for i in range(n):
        temp = defaultdict(float)
        rng = list(range(initial_counts1[i] + 1)) * n
        allocations = stars_and_bars(initial_counts1[i], n)
        for key, value in prev.items():
            for allocation in allocations:
                temp[tuple(map(lambda x, y: x + y, key, allocation))] += value * multinomial(initial_counts1[i], allocation, probs1[i])
        prev = temp
    return prev


def stars_and_bars(n: int, k: int) -> list[tuple[int]]:
    if k == 1:
        return [(n,)]
    
    result = []
    for i in range(n + 1):
        for combo in stars_and_bars(n - i, k - 1):
            result.append((i,) + combo)
    return result

def plot_action_probs(initial_counts: tuple[int], probs: tuple[tuple[float]]):
    """Plot the action probabilities as a bar chart."""
    result = get_action_probs(initial_counts, probs)
    
    sorted_items = sorted(result.items(), key=lambda x: x[0])
    sorted_items = list(filter(lambda x: x[1] > 1e-4, sorted_items))
    
    # Extract data for plotting
    tuples = [str(item[0]) for item in sorted_items]
    probabilities = [item[1] for item in sorted_items]
    
    # Create the plot
    plt.figure(figsize=(15, 8))
    bars = plt.bar(range(len(tuples)), probabilities, color='skyblue', edgecolor='navy', alpha=0.7)
    
    # Customize the plot
    plt.title(f'Distribution of Action Probabilities\nInitial counts: {initial_counts}, ', 
              fontsize=14, fontweight='bold')
    plt.xlabel('State Tuples\n(counts for each state)', fontsize=10)
    plt.ylabel('Probability', fontsize=12)
    plt.xticks(range(len(tuples)), tuples, rotation=90, ha='right', fontsize=8)
    plt.grid(axis='y', alpha=0.3)
    
    # Add value labels on bars
    for i, (bar, prob) in enumerate(zip(bars, probabilities)):
        plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + max(probabilities)*0.01,
                f'{prob:.2e}', ha='center', va='bottom', fontsize=8, rotation=90)
    
    plt.tight_layout()
    plt.show()
    
    # Print summary statistics
    print(f"Total number of possible outcomes: {len(result)}")
    print(f"Sum of all probabilities: {sum(result.values()):.6f}")
    for i, (tuple_val, prob) in enumerate(sorted_items, 1):
        print(f"{i:2d}. {tuple_val}: {prob:.2e}")

# Test the plotting function
if __name__ == "__main__":
    # Original test case
    print("Original test case:")
    result = get_action_probs(initial_counts=(25, 1, 1), probs=((0.1, 0.5, 0.4), (0.1, 0.5, 0.4), (0.1, 0.5, 0.4)))
    print(f"Number of outcomes: {len(result)}")
    print(f"Sum of probabilities: {sum(result.values()):.6f}")
    
    # Plot the distribution
    plot_action_probs(initial_counts=(25, 1, 1), probs=((0.1, 0.5, 0.4), (0.1, 0.5, 0.4), (0.1, 0.5, 0.4)))
