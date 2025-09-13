#include "dp.h"
#include "rmab.h"

using namespace std;

int main() {
  RMAB rmab;
  rmab.parse();
  rmab.print_args();
  rmab.solve();
  return 0;
}