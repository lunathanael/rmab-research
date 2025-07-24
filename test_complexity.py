import time
import random
import matplotlib.pyplot as plt
from collections import defaultdict
from main import RMAB, find_optimal_policy


def create_test_instance(H, N, alpha, num_states, initial_state):
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
    
    return RMAB(H, N, alpha, initial_state, rewards, transition_probabilities)


def run_complexity_tests():
    test_configs = [
        (2, 2, 0.5, 2, (1, 1)),
        (2, 4, 0.5, 2, (2, 2)),
        (2, 8, 0.5, 2, (4, 4)),
        (2, 10, 0.5, 2, (5, 5)),
        (2, 20, 0.5, 2, (10, 10)),
        (2, 30, 0.5, 2, (15, 15)),
        (2, 40, 0.5, 2, (20, 20)),
        (2, 50, 0.5, 2, (25, 25)),
        (2, 60, 0.5, 2, (30, 30)),
        (2, 2, 0.5, 3, (1, 1, 0)),
        (2, 4, 0.5, 3, (2, 1, 1)),
        (2, 8, 0.5, 3, (4, 2, 2)),
        (2, 10, 0.5, 3, (5, 3, 2)),
        (2, 20, 0.5, 3, (6, 7, 7)),
        (3, 2, 0.5, 2, (1, 1)),
        (3, 4, 0.5, 2, (2, 2)),
        (3, 8, 0.5, 2, (4, 4)),
        (3, 10, 0.5, 2, (5, 5)),
        (3, 20, 0.5, 2, (10, 10)),
        (3, 30, 0.5, 2, (15, 15)),
        (3, 40, 0.5, 2, (20, 20)),
        (3, 50, 0.5, 2, (25, 25)),
        (3, 60, 0.5, 2, (30, 30)),
        (3, 2, 0.5, 3, (1, 1, 0)),
        (3, 4, 0.5, 3, (2, 1, 1)),
        (3, 8, 0.5, 3, (4, 2, 2)),
        (3, 10, 0.5, 3, (3, 3, 4)),
        (3, 20, 0.5, 3, (6, 7, 7)),
        (4, 2, 0.5, 2, (1, 1)),
        (4, 4, 0.5, 2, (2, 2)),
        (4, 8, 0.5, 2, (4, 4)),
        (4, 10, 0.5, 2, (5, 5)),
        (4, 20, 0.5, 2, (10, 10)),
        (4, 30, 0.5, 2, (15, 15)),
        (4, 40, 0.5, 2, (20, 20)),
        (4, 50, 0.5, 2, (25, 25)),
        (4, 60, 0.5, 2, (30, 30)),
        (4, 2, 0.5, 3, (1, 1, 0)),
        (4, 4, 0.5, 3, (2, 1, 1)),
        (4, 8, 0.5, 3, (4, 2, 2)),
        (4, 10, 0.5, 3, (3, 3, 4)),
        (4, 20, 0.5, 3, (6, 7, 7)),
        (5, 2, 0.5, 2, (1, 1)),
        (5, 4, 0.5, 2, (2, 2)),
        (5, 8, 0.5, 2, (4, 4)),
        (5, 10, 0.5, 2, (5, 5)),
        (5, 20, 0.5, 2, (10, 10)),
        (5, 30, 0.5, 2, (15, 15)),
        (5, 40, 0.5, 2, (20, 20)),
        (5, 50, 0.5, 2, (25, 25)),
        (5, 60, 0.5, 2, (30, 30)),
        (5, 2, 0.5, 3, (1, 1, 0)),
        (5, 4, 0.5, 3, (2, 1, 1)),
        (5, 8, 0.5, 3, (4, 2, 2)),
        (5, 10, 0.5, 3, (3, 3, 4)),
        (5, 20, 0.5, 3, (6, 7, 7)),

        
        (2, 2, 0.5, 4, (1, 1, 0, 0)),
        (2, 4, 0.5, 4, (2, 1, 1, 1)),
        (2, 8, 0.5, 4, (2, 2, 2, 2)),
        (2, 10, 0.5, 4, (3, 2, 2, 3)),

        
        (4, 2, 0.5, 4, (1, 1, 0, 0)),
        (4, 4, 0.5, 4, (2, 1, 1, 1)),
        (4, 8, 0.5, 4, (2, 2, 2, 2)),
        (4, 10, 0.5, 4, (3, 2, 2, 3)),
    ]
    
    results = []
    
    print("Running complexity tests...")
    for i, (H, N, alpha, num_states, initial_state) in enumerate(test_configs):
        print(f"Test {i+1}/{len(test_configs)}: H={H}, N={N}, num_states={num_states}")
        
        try:
            rmab = create_test_instance(H, N, alpha, num_states, initial_state)
            
            start_time = time.time()
            optimal_value, _ = find_optimal_policy(rmab, verbose=False)
            elapsed_time = time.time() - start_time
            
            theoretical_ops = H * N ** (4 * (num_states - 1))
            
            results.append({
                'H': H,
                'N': N,
                'alpha': alpha,
                'num_states': num_states,
                'initial_state': initial_state,
                'actual_time': elapsed_time,
                'theoretical_ops': theoretical_ops,
                'optimal_value': optimal_value,
                'state_space_size': len(rmab.all_states)
            })
            
            print(f"  Completed in {elapsed_time:.2f}s")
            print(f"  State space: {len(rmab.all_states)} states")
            print(f"  Optimal value: {optimal_value:.4f}")
            
        except Exception as e:
            print(f"  Failed: {e}")
            continue
    
    return results


def plot_results(results):
    results_by_config = {}
    for result in results:
        config_key = f"H={result['H']}, states={result['num_states']}"
        if config_key not in results_by_config:
            results_by_config[config_key] = []
        results_by_config[config_key].append(result)
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
    
    colors = ['blue', 'red', 'green', 'orange', 'purple', 'brown', 'pink', 'gray']
    
    for i, (config, config_results) in enumerate(results_by_config.items()):
        N_values = [r['N'] for r in config_results]
        times = [r['actual_time'] for r in config_results]
        
        ax1.plot(N_values, times, 'o-', color=colors[i % len(colors)], 
                label=config, linewidth=2, markersize=6)
    
    ax1.set_xlabel('N (Total Resources)')
    ax1.set_ylabel('Computation Time (seconds)')
    ax1.set_title('Actual Computation Times')
    ax1.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    ax1.grid(True, alpha=0.3)
    ax1.set_yscale('log')
    
    for i, (config, config_results) in enumerate(results_by_config.items()):
        N_values = [r['N'] for r in config_results]
        theoretical_ops = [r['theoretical_ops'] for r in config_results]
        
        ax2.plot(N_values, theoretical_ops, 's-', 
                color=colors[i % len(colors)], 
                label=config, linewidth=2, markersize=6)
    
    ax2.set_xlabel('N (Total Resources)')
    ax2.set_ylabel('Theoretical Operations')
    ax2.set_title('Theoretical Time Complexity')
    ax2.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    ax2.grid(True, alpha=0.3)
    ax2.set_yscale('log')
    
    plt.tight_layout()
    plt.savefig('complexity_analysis.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    print("\n" + "="*60)
    print("COMPLEXITY ANALYSIS SUMMARY")
    print("="*60)
    
    for config, config_results in results_by_config.items():
        print(f"\nFor {config}:")
        print(f"{'N':>4} {'Time(s)':>8} {'States':>8} {'Ops':>12} {'Ops/sec':>10}")
        print("-" * 50)
        
        for result in config_results:
            ops_per_sec = result['theoretical_ops'] / result['actual_time']
            print(f"{result['N']:>4} {result['actual_time']:>8.2f} "
                  f"{result['state_space_size']:>8} "
                  f"{result['theoretical_ops']:>12,.0f} "
                  f"{ops_per_sec:>10,.0f}")


def analyze_scalability():
    print("\n" + "="*60)
    print("SCALABILITY ANALYSIS")
    print("="*60)
    
    N_values = [10, 20, 30, 40, 50, 60]
    configs = [(2, 2), (3, 2), (4, 2), (5, 2), (2, 3), (3, 3), (4, 3), (5, 3), (2, 4)]
    
    for H, num_states in configs:
        print(f"\nH={H}, num_states={num_states} scaling analysis:")
        times = []
        ops = []
        
        for N in N_values:
            try:
                if H * N ** (4 * (num_states - 1)) > 10 ** 12:
                    print(f"  N={N:2d}: Skipping - too large")
                    continue
                
                base = N // num_states
                remainder = N % num_states
                initial_state = tuple([base + (1 if i < remainder else 0) 
                                     for i in range(num_states)])
                
                rmab = create_test_instance(H, N, 0.5, num_states, initial_state)
                start_time = time.time()
                find_optimal_policy(rmab, verbose=False)
                elapsed = time.time() - start_time
                
                theoretical_ops = H * N ** (4 * (num_states - 1))
                times.append(elapsed)
                ops.append(theoretical_ops)
                
                print(f"  N={N:2d}: {elapsed:6.2f}s, {theoretical_ops:10,.0f} ops")
                
            except Exception as e:
                print(f"  N={N:2d}: Failed - {e}")
                break
        
        if len(times) > 1:
            print(f"  Time growth rate: {times[-1]/times[0]:.2f}x")
            print(f"  Ops growth rate: {ops[-1]/ops[0]:.2f}x")
            expected_growth = N_values[-1] ** (4 * (num_states - 1)) / N_values[0] ** (4 * (num_states - 1))
            print(f"  Expected growth: {expected_growth:.2f}x")


if __name__ == "__main__":
    print("Starting Complexity Analysis")
    print("="*60)
    
    results = run_complexity_tests()
    
    if results:
        plot_results(results)
        analyze_scalability()
        print("\nAnalysis complete! Generated complexity_analysis.png")
    else:
        print("No successful tests completed.")