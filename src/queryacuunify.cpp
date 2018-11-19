#include "queryacuunify.h"
#include "parse.h"
#include "factories.h"
#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <vector>
#include <tuple>
#include "factories.h"
#include "variable.h"
#include "varterm.h"
#include "funterm.h"

using namespace std;

QueryACUUnify::QueryACUUnify() {}
  
Query *QueryACUUnify::create() {
  return new QueryACUUnify();
}
  
void QueryACUUnify::parse(string &s, int &w) {
  matchString(s, w, "acu-unify");
  skipWhiteSpace(s, w);
  t1 = parseTerm(s, w);
  skipWhiteSpace(s, w);
  t2 = parseTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");
  t1->vars(); t2->vars();
}

int gcd(int a, int b) {
  while (b) {
    int c = b;
    b = a % b;
    a = c;
  }
  return a;
}

// returns a^-1 mod m, when gcd(a, m) == 1
int modInv(int a, int m) {
  return a == 1 ? 1 : (1 - modInv(m % a, a) * m) / a + m;
}

// solves a * x == b * y + c * z + v, when v == 0
// returns the set of minimal solutions (a basis, in other words)
vector<tuple<int, int, int>> solve3WithV0(int a, int b, int c) {
  // returns a number <mb>, such that gcd(a, b) == ma * a + <mb> * b
  // this is the same as saying that mb is the inverse of b / gcd(a, b) in Z_{ymax}
  auto getMultiplier = [&](int a, int b, int ymax) {
    return modInv(b / gcd(a, b), ymax);
  };

  vector<tuple<int, int, int>> minSolutions;
  int gb = gcd(a, b);
  int gc = gcd(a, c);
  int gabc = gcd(gb, c);
  int ymax = a / gb;
  int zmax = a / gc;
  int dz = gb / gabc;
  int dy = (c * getMultiplier(a, b, ymax) / gabc) % ymax;
  int y = ymax - dy;
  int z = dz;
  minSolutions.emplace_back(b / gb, ymax, 0);
  minSolutions.emplace_back(c / gc, 0, zmax);
  minSolutions.emplace_back((b * y + c * z) / a, y, z);

  while (dy > 0) {
    while (y > dy) {
      y -= dy;
      z += dz;
      minSolutions.emplace_back((b * y + c * z) / a, y, z);
    }
    dz += dy * z / y;
    dy %= y;
  }

  return minSolutions;
}

void QueryACUUnify::execute() {
  cout << "ACU-Unifying " << t1->toString() << " and " << t2->toString() << endl;
  for(auto it : solve3WithV0(3, 2, 1)) {
    cout << get<0>(it) << ' ' << get<1>(it) << ' ' << get<2>(it) << '\n';
  }
}