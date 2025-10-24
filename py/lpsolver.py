import numpy as np
import pulp as pulp
from pulp import *

def solve_lp_standard(P0,P1,R0,R1,alpha_list,T,init,print_res=False):
    n = len(R0)
    P = [P0,P1]
    R = [R0,R1]
    action = range(0,2)
    state = range(0,n)
    horizon = range(0,T)
    prob = LpProblem("LP1", LpMaximize)
    variables = LpVariable.dicts("Y",(horizon,action,state),lowBound=0., upBound=1.)
    # resource constraints
    for t in horizon:
        prob += lpSum([variables[t][1][s] for s in state]) == alpha_list[t]
    # Markov state evolution
    for t in range(0,T-1):
        for s in state:
            prob += lpSum(variables[t+1][a][s] for a in action) == lpSum([variables[t][a][ss]*P[a][ss][s] for a in action for ss in state])
    # initial condition        
    for s in state:
        prob += lpSum(variables[0][a][s] for a in action) == init[s]
    # objective    
    prob += lpSum([variables[t][a][s]*R[a][s] for t in horizon for a in action for s in state])
    
    prob.solve()
    
    y = np.zeros((T,2,n))
    for t in horizon:
        for a in action:
            for s in state:
                V = variables[t][a][s]
                y[t,a,s] = V.varValue

    upper = value(prob.objective)
    if print_res:
        print("Optim Value is " + str(upper))
        return y, upper
    else:
        return y
    
def randomization_numbers(P0,P1,R0,R1,alpha_list,T,init):
    n = len(R0)
    P = [P0,P1]
    R = [R0,R1]
    action = range(0,2)
    state = range(0,n)
    horizon = range(0,T)
    prob = LpProblem("LP1", LpMaximize)
    variables = LpVariable.dicts("Y",(horizon,action,state),lowBound=0., upBound=1.)
    # resource constraints
    for t in horizon:
        prob += lpSum([variables[t][1][s] for s in state]) == alpha_list[t]
    # Markov state evolution
    for t in range(0,T-1):
        for s in state:
            prob += lpSum(variables[t+1][a][s] for a in action) == lpSum([variables[t][a][ss]*P[a][ss][s] for a in action for ss 
                                                                          in state])
    # initial condition        
    for s in state:
        prob += lpSum(variables[0][a][s] for a in action) == init[s]
    # objective    
    prob += lpSum([variables[t][a][s]*R[a][s] for t in horizon for a in action for s in state])
    
    prob.solve()
    
    y = np.zeros((T,2,n))
    randomization_nb = np.zeros(T, dtype=int)
    for t in horizon:
        for a in action:
            for s in state:
                V = variables[t][a][s]
                y[t,a,s] = V.varValue
    for t in horizon:
        randomization_count = 0
        for s in state:
            if y[t,0,s] > 1e-8 and y[t,1,s] > 1e-8:
                randomization_count += 1
        randomization_nb[t] = randomization_count
    return randomization_nb


def solve_lp_infinite_horizon(P0, P1, R0, R1, alpha, n_states):
    """
    Solves the infinite-horizon LP relaxation (Eq. 5 from the paper)
    and returns the dual variables of the steady-state constraint, which
    serve as the priority indices.
    """
    prob = LpProblem("InfiniteHorizonLP", LpMaximize)

    # Primal variables: x_i and u_i
    x = LpVariable.dicts("x", range(n_states), lowBound=0)
    u = LpVariable.dicts("u", range(n_states), lowBound=0)

    # Objective function (5a)
    prob += lpSum([R0[i] * x[i] + (R1[i] - R0[i]) * u[i] for i in range(n_states)])

    # Constraints (5b)
    for i in range(n_states):
        prob += u[i] <= x[i]
    prob += lpSum(u.values()) <= alpha
    prob += lpSum(x.values()) == 1 # x must be a probability distribution

    # Steady-state constraint (5c)
    # x_j = sum_i(x_i * P0_ij) + sum_i(u_i * (P1_ij - P0_ij))
    steady_state_constraints = {}
    for j in range(n_states):
        constraint = lpSum(x[j]) == lpSum([x[i] * P0[i, j] + u[i] * (P1[i, j] - P0[i, j]) for i in range(n_states)])
        prob += constraint
        steady_state_constraints[j] = constraint
        
    prob.solve()

    # Extract the dual variables (shadow prices) for the steady-state constraints
    priority_indices = [steady_state_constraints[j].pi for j in range(n_states)]
    
    return priority_indices