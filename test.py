def generate_actions(s: tuple[int], alpha_n, S) -> set[tuple[int]]:
    actions = [(0, tuple())]
    pulls = alpha_n
    for i in range(S - 1):
        tmp = []
        for j in range(min(s[i], pulls) + 1):
            for k in actions:
                if k[0] + j <= pulls:
                    tmp.append((k[0] + j, k[1] + (j,)))
        actions = tmp
    actions = set(x[1] + (pulls - x[0],) for x in actions if pulls - x[0] <= s[-1])
    return actions

def stars_and_bars(n: int, k: int) -> list[tuple[int]]:
    if k == 1:
        return [(n,)]
    
    result = []
    for i in range(n + 1):
        for combo in stars_and_bars(n - i, k - 1):
            result.append((i,) + combo)
    return result


all_states = stars_and_bars(10, 4)
actions = []
for state in all_states:
    actions.append(generate_actions(state, 5, 4))

print(actions)
print(len(actions))
print(sum(len(x) for x in actions))