#include "dp.h"
#include "rmab.h"

using namespace std;

int main() {
  RMAB rmab;
  rmab.parse();
  rmab.print_args();
  for(int i : {10, 20})
    cout << format("#Arms: {}, Opt: {}\n", i, rmab.solve(i));
  return 0;
}