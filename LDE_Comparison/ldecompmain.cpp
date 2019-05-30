#include <iostream>
#include <vector>
#include <string>

#include "../ldecompalg.h"
#include "../ldealg.h"

using namespace std;

int main() {
  int x;
  vector<int> l;
  vector<int> r;
  while (cin >> x, x) {
    l.push_back(x);
  }
  while (cin >> x, x) {
    r.push_back(x);
  }
  LDECompAlg alg(l, r, 0, true);
  cout << alg.solve().size() << '\n';
  return 0;
}
