import numpy as np
import subprocess
from py.lpsolver import randomization_numbers, solve_lp_standard, solve_lp_infinite_horizon

N_STATES = 3
T_HORIZON = 3
CPP_EXECUTABLE = "./build/RMAB"  # Path to your compiled C++ executable
INIT_STATE = [0.3, 0.3, 0.4]
N_VALUES_TO_TEST = [10, 20, 30, 40, 50]
DISCOUNT_FACTOR_ALPHA = 0.5

def generate_random_inputs(n, T):
    """Generates random valid inputs for the LP solver functions."""
    P0 = np.random.rand(n, n)
    P0 /= P0.sum(axis=1, keepdims=True)
    
    P1 = np.random.rand(n, n)
    P1 /= P1.sum(axis=1, keepdims=True)
    
    R0 = np.random.rand(n)
    R1 = np.random.rand(n)
    
    init = np.array(INIT_STATE) * n
    
    alpha_list = np.array([DISCOUNT_FACTOR_ALPHA] * T)

    if np.any(randomization_numbers(P0, P1, R0, R1, alpha_list, T, init) != np.ones(T)):
        return generate_random_inputs(n, T)
    
    return P0, P1, R0, R1, alpha_list, T, init

def get_priority_policy_actions(state_counts, priority_indices, alpha):
    """
    Determines the action for a given state distribution based on a priority policy.

    Args:
        state_counts (list): A list where state_counts[i] is the number of arms in state i.
        priority_indices (list): A list of priority values for each state.
        alpha (float): The budget constraint.

    Returns:
        list: A list where list[i] is the number of arms to pull in state i.
    """
    n_states = len(state_counts)
    total_arms = sum(state_counts)
    budget = int(np.floor(alpha * total_arms))

    # Create a list of all arms, represented by (state_index, priority_index)
    all_arms = []
    for state_idx in range(n_states):
        count = state_counts[state_idx]
        if count > 0:
            all_arms.extend([(state_idx, priority_indices[state_idx])] * count)

    # Sort arms in descending order of their priority index
    all_arms.sort(key=lambda item: item[1], reverse=True)

    # Decide which arms to pull based on the budget
    actions_to_take = all_arms[:budget]

    # Count how many pulls are assigned to each state
    policy_actions = [0] * n_states
    for state_idx, _ in actions_to_take:
        policy_actions[state_idx] += 1
        
    return policy_actions

def parse_cpp_output(output_string: str) -> list[dict]:
    """Parses the verbose, multi-line output from the C++ executable."""
    parsed_data = []
    lines = output_string.strip().split('\n')
    for i in range(0, len(lines), 3):
        if i + 2 < len(lines):
            try:
                line_t_exp, line_state, line_action = lines[i:i+3]
                t, expectation = line_t_exp.split()
                state = [int(s) for s in line_state.split()]
                action = [int(a) for a in line_action.split()]
                parsed_data.append({
                    't': int(t), 'expectation': float(expectation),
                    'state': state, 'action': action
                })
            except (ValueError, IndexError):
                continue
    return parsed_data

def format_cpp_input(T, n, alpha, init, P0, P1, R0, R1):
    """Formats variables for the C++ executable's input."""
    lines = [f"{T} {n} {alpha}", " ".join(map(str, init))]
    for _ in range(T):
        for s in range(n):
            lines.append(f"{R0[s]} {R1[s]}")
    for _ in range(T):
        for row in P0: lines.append(" ".join(map(str, row)))
        for row in P1: lines.append(" ".join(map(str, row)))
    return "\n".join(lines)

def run_cpp_and_evaluate_policy(P0, P1, R0, R1, alpha, T, n_states, n_value):
    """
    Runs the C++ solver and compares its optimal actions against the priority policy.
    """
    # 1. Calculate the priority indices for this problem instance
    print("\n--- Solving infinite-horizon LP to get priority indices ---")
    priority_indices = solve_lp_infinite_horizon(P0, P1, R0, R1, alpha, n_states)
    print(f"Priority Indices: {priority_indices}")

    # 2. Run the C++ executable to get optimal actions for various initial states
    print(f"\n--- Running C++ solver for N = {n_value} to get optimal actions ---")
    init = np.ones(n_states) / n_states # Initial state for C++ doesn't matter for this test
    cpp_input_params = format_cpp_input(T, n_states, alpha, init, P0, P1, R0, R1)
    full_input = cpp_input_params + "\n" + str(n_value)
    
    try:
        process = subprocess.run([CPP_EXECUTABLE], input=full_input, text=True, capture_output=True, check=True)
        optimal_results = parse_cpp_output(process.stdout)
    except Exception as e:
        print(f"Error running C++ executable: {e}")
        return

    # 3. Compare the priority policy against the optimal actions
    if not optimal_results:
        print("No output from C++ solver to evaluate.")
        return

    match_count = 0
    total_cases = len(optimal_results)
    
    print("\n--- Evaluating Priority Policy against C++ Optimal Actions ---")
    for result in optimal_results:
        state_from_cpp = result['state']
        optimal_action_from_cpp = result['action']
        
        # Get the action recommended by our simple policy
        policy_action = get_priority_policy_actions(state_from_cpp, priority_indices, alpha)
        
        # Check if they match
        if policy_action == optimal_action_from_cpp:
            match_count += 1
        # else:
        #     print(f"Mismatch for state {state_from_cpp}:")
        #     print(f"  Optimal: {optimal_action_from_cpp}")
        #     print(f"  Policy:  {policy_action}")

    print("\n--- Results ---")
    print(f"Priority policy actions matched the optimal C++ actions in {match_count} out of {total_cases} cases.")
    if total_cases > 0:
        accuracy = (match_count / total_cases) * 100
        print(f"Match Rate: {accuracy:.2f}%")


def main():
    """Main workflow orchestrator."""
    print("--- Generating random problem instance ---")
    P0, P1, R0, R1, _, T, _ = generate_random_inputs(N_STATES, T_HORIZON)
    
    # Run the evaluation for each N value specified
    for n in N_VALUES_TO_TEST:
        run_cpp_and_evaluate_policy(P0, P1, R0, R1, DISCOUNT_FACTOR_ALPHA, T, N_STATES, n)

if __name__ == "__main__":
    main()