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