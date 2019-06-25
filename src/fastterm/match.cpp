#include "fastterm.h"
#include "fastqueryacunify.h"
#include <cassert>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <string.h>
using namespace std;

bool matchList(FastTerm *tl1, FastTerm *tl2, uint32 count, FastSubst &subst)
{
  for (uint i = 0; i < count; ++i) {
    if (!match(tl1[i], tl2[i], subst).size()) {
      return false;
    }
  }
  return true;
}

vector<FastSubst> match(FastTerm subject, FastTerm pattern, FastSubst &subst) {
  vector<FastVar> vars;
  function<void(FastTerm)> getVars = [&](FastTerm t) {
    if (isVariable(t)) {
      vars.push_back(t);
      return;
    }
    for (uint32 i = 0; i < getArity(getFunc(t)); ++i) {
      getVars(getArg(t, i));
    }
  };
  getVars(subject);
  sort(vars.begin(), vars.end());
  vars.erase(unique(vars.begin(), vars.end()), vars.end());
  vector<FastTerm> consts(vars.size());
  for (uint i = 0; i < vars.size(); ++i) {
    char constName[20];
    strcpy(constName, ("_c" + to_string(i)).c_str());
    consts[i] = newFuncTerm(newConst(constName, fastStateSort()), nullptr);
    subject = applyUnitySubst(subject, vars[i], consts[i]);
    pattern = applyUnitySubst(pattern, vars[i], consts[i]);
  }
  auto minSubstSet = FastQueryACUnify(subject, pattern).solve();
  for (auto &s : minSubstSet) {
    for (uint i = 0; i < vars.size(); ++i) {
      s.replaceConstWithVar(consts[i], vars[i]);
    }
  }
  if (minSubstSet.size()) {
    subst = minSubstSet[0];
  }
  return minSubstSet;
}
