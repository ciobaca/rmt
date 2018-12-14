#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <numeric>
#include "ldegraphalg.h"

using namespace std;

LDEGraphAlg::LDEGraphAlg() {}

LDEGraphAlg::LDEGraphAlg(const vector<int> &a, const vector<int> &b, int c) {
  if ((this->a.size() && *min_element(a.begin(), a.end()) <= 0) || (this->b.size() && *min_element(b.begin(), b.end()) <= 0)) {
    cout << "Wrong parameters: 'a' or 'b' contains a coefficient less or equal to zero" << endl;
    exit(0);
  }

  this->a = a;
  this->b = b;
  this->c = c;
  this->offset = max(*max_element(a.begin(), a.end()), abs(c));
  this->viz.resize(offset + max(*max_element(b.begin(), b.end()), abs(c)) + 1);
  this->partialSol.resize(a.size() + b.size());
  this->sumPartialSol = 0;
}

bool LDEGraphAlg::isMinimal(int diff, int apos, int bpos) {
  if (sumPartialSol == 1) {
    return true;
  }
  if (diff >= 0) {
    for (int i = apos; i < (int)a.size(); ++i) {
      if (partialSol[i]) {
        if (diff == a[i]) {
          return false;
        }
        --partialSol[i];
        --sumPartialSol;
        bool temp = isMinimal(diff - a[i], i, bpos);
        ++partialSol[i];
        ++sumPartialSol;
        if (!temp) {
          return false;
        }
      }
    }
  } else {
    for (int i = bpos; i < (int)b.size(); ++i) {
      if(partialSol[i + a.size()]) {
        if (diff == -b[i]) {
          return false;
        }
        --partialSol[i + a.size()];
        --sumPartialSol;
        bool temp = isMinimal(diff + b[i], apos, i);
        ++partialSol[i + a.size()];
        ++sumPartialSol;
        if (!temp) {
          return false;
        }
      }
    }
  }
  return true;
}

void LDEGraphAlg::graphAlg(int diff, int apos, int bpos) {
  if (offset + diff >= (int)viz.size() || offset + diff < 0) {
    return;
  }

  if (!diff) {
    if ((!this->c && viz[offset] == 1) || this->c) {
      if (isMinimal(0, 0, 0)) {
        addSolution(partialSol);
      }
      return;
    }
  }

  if (viz[offset + diff]) {
    return;
  }

  ++viz[offset + diff];

  if (diff >= 0) {
    for (int i = apos; i < (int)a.size(); ++i) {
      ++partialSol[i];
      ++sumPartialSol;
      graphAlg(diff - a[i], i, bpos);
      --partialSol[i];
      --sumPartialSol;
    }
  } else {
    for (int i = bpos; i < (int)b.size(); ++i) {
      ++partialSol[i + a.size()];
      ++sumPartialSol;
      graphAlg(diff + b[i], apos, i);
      --partialSol[i + a.size()];
      --sumPartialSol;
    }
  }

  --viz[offset + diff];
}

vector<pair<vector<int>, vector<int>>> LDEGraphAlg::solve() {
  basis.clear();
  fill(viz.begin(), viz.end(), 0);
  sumPartialSol = 0;
  graphAlg(c, 0, 0);
  return this->basis;
}