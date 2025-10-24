#include "dp.h"
#include "rmab.h"

using namespace std;

int main() {
  RMAB rmab;
  rmab.parse();
  int i;
  while(cin >> i) {
    auto res = rmab.solve(i, true, true);
  }
  // rmab.print_args();
  // for (int i : {50}) {
  //   auto res = rmab.solve(i, true);
    // cout << format("#Arms: {}, Opt Total: {}, Opt Per Arm: {}\n", i, res.expectation, res.expectation / i);
    // cout << format("Optimal First Action:\n");
    // for (int i : res.best_action.to_vector()) {
    //   cout << i << ' ';
    // }
  //   cout << '\n';
  // }
  return 0;
}