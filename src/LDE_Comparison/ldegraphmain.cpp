#include <iostream>
#include <vector>
#include <cstring>
//#include <fstream>
#include <chrono>
#include <iomanip>
#include <algorithm>

#include "../ldeslopesalg.h"
#include "../ldegraphalg.h"
#include "../ldealg.h"

using namespace std;

int main() {
  //  ifstream input("fullTests.txt");
  /// ofstream output("graphResults.txt");
  int n, m, x;
  n = m = 0;
  vector<int> l;
  vector<int> r;
  
  while (1) {
    cin >> x;
    if (x == 0) {
      break;
    }
    l.push_back(x);
  }
  while (1) {
    cin >> x;
    if (x == 0) {
      break;
    }
    r.push_back(x);
  }
   
  LDEGraphAlg alg(l, r);
  // chrono::high_resolution_clock::time_point t1, t2;
  // t1 = chrono::high_resolution_clock::now();
  auto ans = alg.solve();
  cout << ans.size() << endl;
  // t2 = chrono::high_resolution_clock::now();
  // output << fixed << setprecision(12) << chrono::duration_cast<chrono::microseconds>(t2 - t1).count() / 1e6 << '\n';

  return 0;
}
