#include <iostream>
#include <vector>
#include <string>

#include "../ldelexalg.h"
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
  LDELexAlg alg(l, r, 0, 1);
  cout << alg.solve().size() << '\n';
  return 0;
}
