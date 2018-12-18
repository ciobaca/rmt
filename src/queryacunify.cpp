#include "queryacunify.h"
#include "parse.h"
#include "factories.h"
#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <vector>
#include <tuple>
#include <queue>
#include <functional>
#include "factories.h"
#include "variable.h"
#include "varterm.h"
#include "funterm.h"
#include "ldecompalg.h"
#include "ldelexalg.h"
#include "ldegraphalg.h"
#include "ldeslopesalg.h"

using namespace std;

QueryACUnify::QueryACUnify() {}
  
Query *QueryACUnify::create() {
  return new QueryACUnify();
}
  
void QueryACUnify::parse(string &s, int &w) {
  matchString(s, w, "ac-unify");
  skipWhiteSpace(s, w);
  t1 = parseTerm(s, w);
  skipWhiteSpace(s, w);
  t2 = parseTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");
  t1->vars(); t2->vars();
}

void QueryACUnify::execute() {
  Function *f = getFunction("f");
  function<void(Term*, map<Term*, int>&)> getCoefs = [&](Term *t, map<Term*, int> &M) {
    if(t->isVarTerm()) {
      ++M[t];
      return;
    }
    for(auto term : t->getAsFunTerm()->arguments) {
      getCoefs(term, M);
    }
  };
  auto delSameCoefs = [&](map<Term*, int> &l, map<Term*, int> &r) {
    bool swaped = false;
    if (l.size() > r.size()) {
      swaped = true;
      l.swap(r);
    }
    vector<Term*> toDel;
    for (auto it : l) {
      if (r.count(it.first)) {
        toDel.push_back(it.first);
      }
    }
    for (Term *it : toDel) {
      int minVal = min(l[it], r[it]);
      l[it] -= minVal;
      r[it] -= minVal;
      if (!l[it]) {
        l.erase(it);
      }
      if (!r[it]) {
        r.erase(it);
      }
    }
  };
  auto fromMapToVector = [&](map<Term*, int> &M) {
    vector<int> ans;
    for (auto it : M) {
      ans.push_back(it.second);
    }
    return ans;
  };
  auto createFuncWithSameVar = [&] (int cnt, Term* var) {
    if (!cnt) {
      return getFunTerm(getFunction("e"), {});
    }
    --cnt;
    if (!cnt) {
      return var;
    }
    Term *ans = var;
    while (cnt > 0) {
      --cnt;
      ans = getFunTerm(f, {ans, var});
    }
    return ans;
  };

  cout << "AC-Unifying " << t1->toString() << " and " << t2->toString() << endl;
  map<Term*, int> l, r;
  getCoefs(t1, l);
  getCoefs(t2, r);
  delSameCoefs(l, r);
  vector<int> a = fromMapToVector(l);
  vector<int> b = fromMapToVector(r);
  LDEGraphAlg graphAlg(a, b);
  vector<pair<vector<int>, vector<int>>> result = graphAlg.solve();
  if (!result.size()) {
    cout << "No unification" << endl;
    return;
  }

  if (result.size() > 64) {
    cout << "Sorry, but the answer might be too large." << endl;
    return;
  }

  vector<Substitution> sigma;
  int varId = 0;
  for (auto sol : result) {
    int index = 0;
    sigma.push_back(Substitution());
    string varName = "_Z_" + to_string(varId);
    createVariable(varName, (Sort*)getSort("State"));
    Term *z = getVarTerm(getVariable(varName));
    for (auto it : l) {
      sigma.back().add(it.first->getAsVarTerm()->variable, createFuncWithSameVar(sol.first[index], z));
      ++index;
    }
    index = 0;
    for (auto it : r) {
      sigma.back().add(it.first->getAsVarTerm()->variable, createFuncWithSameVar(sol.second[index], z));
      ++index;
    }
    ++varId;
  }

  using uint64 = unsigned long long;
  function<bool(uint64, Substitution&)> getSubstFromMask = [&](uint64 mask, Substitution &subst) {
    for (auto it : l) {
      Term *ans = NULL;
      for (int i = 0; i < (int)sigma.size(); ++i) {
        if (mask & (1LL << i)) {
          continue;
        }
        Term *aux = sigma[i].image(it.first->getAsVarTerm()->variable);
        if (aux->isVarTerm() || aux->getAsFunTerm()->toString() != "e") {
          ans = ans ? getFunTerm(f, {ans, aux}) : aux;
        }
      }
      if (ans) {
        subst.add(it.first->getAsVarTerm()->variable, ans);
      } else {
        return false;
      }
    }
    for (auto it : r) {
      Term *ans = NULL;
      for (int i = 0; i < (int)sigma.size(); ++i) {
        if (mask & (1LL << i)) {
          continue;
        }
        Term *aux = sigma[i].image(it.first->getAsVarTerm()->variable);
        if (aux->isVarTerm() || aux->getAsFunTerm()->toString() != "e") {
          ans = ans ? getFunTerm(f, {ans, aux}) : aux;
        }
      }
      if (ans) {
        subst.add(it.first->getAsVarTerm()->variable, ans);
      } else {
        return false;
      }
    }
    return true;
  };

  vector<Substitution> minSubstSet;
  minSubstSet.push_back(Substitution());
  if (!getSubstFromMask(0, minSubstSet.back())) {
    cout << "No unification" << endl;
    return;
  }

  queue<uint64> q;
  for (q.push(0); !q.empty(); q.pop()) {
    uint64 now = q.front();
    for (int i = 0; i < (int)sigma.size(); ++i) {
      if (!(now & (1LL << i))) {
        Substitution subst;
        if (getSubstFromMask(now | (1LL << i), subst)) {
          minSubstSet.push_back(subst);
          q.push(now | (1LL << i));
        }
      }
    }
  }

  sort(minSubstSet.begin(), minSubstSet.end());
  minSubstSet.erase(unique(minSubstSet.begin(), minSubstSet.end()), minSubstSet.end());
  for (auto &subst : minSubstSet) {
    cout << subst.toString() << endl;
  }
}