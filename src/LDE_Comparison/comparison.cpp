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
  int n, m, test = 1;
  while (input >> n >> m) {
    vector<int> l(n);
    vector<int> r(m);
    for (int &x : l) {
      cin >> x;
    }
    for (int &x : r) {
      cin >> x;
    }
    // cerr << "Test #" << test++ << '\n';
    // cerr << n << ' ' << m << ' ' << max(*max_element(l.begin(), l.end()), *max_element(r.begin(), r.end())) << '\n';
    
    LDEGraphAlg alg(l, r);
    // chrono::high_resolution_clock::time_point t1, t2;
    // t1 = chrono::high_resolution_clock::now();
    auto ans = alg.solve();
    // t2 = chrono::high_resolution_clock::now();
    // output << fixed << setprecision(12) << chrono::duration_cast<chrono::microseconds>(t2 - t1).count() / 1e6 << '\n';
  }
  return 0;
}

