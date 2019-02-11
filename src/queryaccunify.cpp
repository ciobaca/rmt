#include "queryaccunify.h"
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
#include <chrono>
#include "factories.h"
#include "variable.h"
#include "varterm.h"
#include "funterm.h"
#include "ldecompalg.h"
#include "ldelexalg.h"
#include "ldegraphalg.h"
#include "ldeslopesalg.h"

using namespace std;
using namespace std::chrono;

QueryACCUnify::QueryACCUnify() {}
  
Query *QueryACCUnify::create() {
  return new QueryACCUnify();
}
  
void QueryACCUnify::parse(string &s, int &w) {
  matchString(s, w, "acc-unify");
  skipWhiteSpace(s, w);
  t1 = parseTerm(s, w);
  skipWhiteSpace(s, w);
  t2 = parseTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");
  t1->vars(); t2->vars();
}

void QueryACCUnify::execute() {
  Function *f = getFunction("f");
  Term* unityElement = getFunTerm(f->unityElement, {});
  function<void(Term*, map<Term*, int>&, map<Term*, Term*>&)> getCoefs = [&](Term *t, auto &M, auto &constToVar) {
    if(t->isVarTerm) {
      ++M[t];
      return;
    }
    FunTerm *funTerm = t->getAsFunTerm();
    if(!funTerm->arguments.size()) {
      if(!constToVar.count(funTerm)) {
        constToVar[funTerm] = getVarTerm(createFreshVariable((Sort*) getSort("State")));
      }
      ++M[constToVar[funTerm]];
      return;
    }
    for(auto term : funTerm->arguments) {
      getCoefs(term, M, constToVar);
    }
  };
  auto delSameCoefs = [&](map<Term*, int> &l, map<Term*, int> &r) {
    if (l.size() > r.size()) {
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
  auto createFuncWithSameVar = [&](int cnt, Term* var) {
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

  cout << "(with constants) AC-Unifying " << t1->toString() << " and " << t2->toString() << endl;
  // TIMEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
  high_resolution_clock::time_point time1, time2;
  time1 = high_resolution_clock::now();

  map<Term*, int> l, r;
  map<Term*, Term*> constToVar;
  getCoefs(t1, l, constToVar);
  getCoefs(t2, r, constToVar);
  delSameCoefs(l, r);
  vector<int> a = fromMapToVector(l);
  vector<int> b = fromMapToVector(r);
  LDEGraphAlg graphAlg(a, b);
  vector<pair<vector<int>, vector<int>>> result = graphAlg.solve();
  if (!result.size()) {
    cout << "No unification" << endl;
    return;
  }

  vector<Substitution> sigma;
  for (const auto &sol : result) {
    int index = 0;
    sigma.push_back(Substitution());
    Term *z = getVarTerm(createFreshVariable((Sort*) getSort("State")));
    for (auto it : l) {
      sigma.back().add(it.first->getAsVarTerm()->variable, createFuncWithSameVar(sol.first[index], z));
      ++index;
    }
    index = 0;
    for (auto it : r) {
      sigma.back().add(it.first->getAsVarTerm()->variable, createFuncWithSameVar(sol.second[index], z));
      ++index;
    }
  }

  vector<vector<Term*>> sigmaImage(sigma.size(), vector<Term*>(l.size() + r.size()));
  for(int i = 0; i < (int)sigma.size(); ++i) {
    int index = 0;
    for(auto it : l) {
      sigmaImage[i][index] = sigma[i].image(it.first->getAsVarTerm()->variable);
      ++index;
    }
    for(auto it : r) {
      sigmaImage[i][index] = sigma[i].image(it.first->getAsVarTerm()->variable);
      ++index;
    }
  }

  auto checkConstConstraints = [&](Substitution &subst) -> bool {
    map<Term*, Term*> constSubst;
    for (const auto &it : constToVar) {
      Variable *var = it.second->getAsVarTerm()->variable;
      if (subst.inDomain(var)) {
        Term *t = subst.image(var);
        if(t->isFunTerm && t != it.second) {
          return false;
        }
        if(t->isVarTerm) {
          if(constSubst.count(t) && constSubst[t] != it.first) {
            return false;
          }
          constSubst[t] = it.first;
        }
      }
    }
    for (const auto &it : constSubst) {
      subst.force(it.first->getAsVarTerm()->variable, it.second);
    }
    return true;
  };
  auto checkMask = [&](const vector<bool> &mask) -> bool {
    vector<bool> ans(sigma.size());
    int n = sigma.size();
    int m = sigmaImage[0].size();
    int cnt = 0;
    for (int i = 0; i < n; ++i) {
      if (mask[i]) {
        continue;
      }
      for (int j = 0; j < m; ++j) {
        if (!ans[j]) {
          Term *aux = sigmaImage[i][j];
          if (aux->isVarTerm || aux != unityElement) {
            ans[j] = true;
            ++cnt;
            if (cnt == m) {
              return true;
            }
          }
        }
      }
    }
    return false;
  };
  auto getSubstFromMask = [&](const vector<bool> &mask, Substitution &subst) -> bool {
    int n = sigma.size();
    int m = sigmaImage[0].size();
    vector<Term*> ans(m);
    for (int i = 0; i < n; ++i) {
      if (mask[i]) {
        continue;
      }
      for (int j = 0; j < m; ++j) {
        Term *aux = sigmaImage[i][j];
        if (aux->isVarTerm || aux != unityElement) {
          ans[j] = ans[j] ? getFunTerm(f, {ans[j], aux}) : aux;
        }
      }
    }
    int index = 0;
    for (auto it : l) {
      subst.add(it.first->getAsVarTerm()->variable, ans[index]);
      ++index;
    }
    for (auto it : r) {
      subst.add(it.first->getAsVarTerm()->variable, ans[index]);
      ++index;
    }
    return checkConstConstraints(subst);
  };

  vector<Substitution> minSubstSet;
  minSubstSet.push_back(Substitution());

  vector<bool> initMask(result.size());
  if (!checkMask(initMask) || !getSubstFromMask(initMask, minSubstSet.back())) {
    minSubstSet.pop_back();
  }

  queue<vector<bool>> q;
  for (q.push(initMask); !q.empty(); q.pop()) {
    vector<bool> &now = q.front();
    for (int i = 0; i < (int)sigma.size() && !now[i]; ++i) {
      now[i] = true;
      if (checkMask(now)) {
        Substitution subst;
        if (getSubstFromMask(now, subst)) {
          minSubstSet.push_back(subst);
        }
      }
      q.push(now);
      now[i] = false;
    }
  }

  map<Term*, Term*> invConstToVar;
  for(const auto &it : constToVar) {
    invConstToVar[it.second] = it.first;
  }

  time2 = high_resolution_clock::now();
  cerr << result.size() << ' ' << minSubstSet.size() << ' ' << duration_cast<microseconds>(time2 - time1).count() / 1e6 << endl;

  if (!minSubstSet.size()) {
    cout << "No unification\n";
  }
  for (auto &subst : minSubstSet) {
    Substitution normalizedSubst;
    for (const auto &it : subst) {
      Term *aux = getVarTerm(it.first);
      if (!invConstToVar.count(aux) && (l.count(aux) || r.count(aux))) {
        normalizedSubst.force(it.first, it.second);
      }
    }
    cout << normalizedSubst.toString() << endl;
  }
}