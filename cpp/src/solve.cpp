#include "rmab.h"
#include "dp.h"
#include "utils.h"

using namespace std;

void RMAB::solve() {
    DPLayer prev(n_arms, n_states), curr(n_arms, n_states);
    DPStateIterator it(n_arms, n_states);
    for(int t = n_steps - 1; t >= 0; --t) {
        it.init();
        for(int i = 0; i < it.end(); i = it.next()) {
            
        }
    }
}