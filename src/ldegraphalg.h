#ifndef LDEGRAPHALG_H__
#define LDEGRAPHALG_H__
#include <vector>
#include <utility>
#include "ldealg.h"

using namespace std;

struct LDEGraphAlg : public LDEAlg {
  LDEGraphAlg();
  LDEGraphAlg(const vector<int> &a, const vector<int> &b, int c = 0);
  void graphAlg(int diff, int apos, int bpos);
  bool isMinimal(int diff, int apos, int bpos);
  vector<pair<vector<int>, vector<int>>> solve();

  private:
    vector<char> viz;
    vector<int> partialSol;
    int offset, sumPartialSol;
};

#endif