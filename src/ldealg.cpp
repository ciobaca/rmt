#include <vector>
#include <utility>
#include <algorithm>
#include "ldealg.h"

using namespace std;

LDEAlg::LDEAlg() {}

bool LDEAlg::isLeq(const pair<vector<int>, vector<int>> &x, const vector<int> &y) {
  for (int i = 0; i < (int)a.size(); ++i) {
    if (x.first[i] > y[i]) {
      return false;
    }
  }
  for (int i = 0; i < (int)b.size(); ++i) {
    if (x.second[i] > y[i + a.size()]) {
      return false;
    }
  }
  return true;
}

void LDEAlg::addSolution(const vector<int> &sol) {
  if (sol != vector<int>(sol.size(), 0)) {
    vector<int> x = vector<int>(sol.begin(), sol.begin() + a.size());
    vector<int> y = vector<int>(sol.begin() + a.size(), sol.end());
    basis.emplace_back(x, y);
  }
}

bool LDEAlg::isMinimal(const vector<int> &sol, int lastPoz) {
  if (lastPoz == -1) {
    lastPoz = basis.size();
  }

  for (int i = 0; i < lastPoz; ++i) {
    if (isLeq(basis[i], sol)) {
      return false;
    }
  }

  return true;
}

