#include <iostream>
#include <queue>
#include <functional>
#include "fastqueryacunify.h"
//#include "parse.h"
#include "unifeq.h"
#include "unifeqsystem.h"
#include "ldegraphalg.h"

using namespace std;

FastQueryACUnify::FastQueryACUnify() {}
  
FastQueryACUnify *FastQueryACUnify::create() {
  return new FastQueryACUnify();
}
  
void FastQueryACUnify::parse(string &s, int &w) {
  /*matchString(s, w, "ac-unify");
  skipWhiteSpace(s, w);
  t1 = parseTerm(s, w);
  skipWhiteSpace(s, w);
  t2 = parseTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");
  t1->vars(); t2->vars();
  if (t1->isFunTerm && t2->isVarTerm) {
    swap(t1, t2);
  }*/
}

void FastQueryACUnify::delSameCoeffs(map<FastTerm, int> &l, map<FastTerm, int> &r) {
  if (l.size() > r.size()) {
    l.swap(r);
  }
  vector<FastTerm> toDel;
  for (auto it : l) {
    if (r.count(it.first)) {
      toDel.push_back(it.first);
    }
  }
  for (auto it : toDel) {
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
}

vector<int> FastQueryACUnify::fromMapToVector(const map<FastTerm, int> &M) {
  vector<int> ans;
  ans.reserve(M.size());
  for (auto it : M) {
    ans.push_back(it.second);
  }
  return ans;
}

FastTerm FastQueryACUnify::createFuncWithSameVar(int cnt, FastTerm var, FastFunc f, FastTerm unityElement) {
  if (!cnt) {
    return unityElement;
  }
  FastTerm ans = var;
  while (cnt > 1) {
    FastTerm p[2] = {ans, var};
    ans = newFuncTerm(f, p);
    --cnt;
  }
  return ans;
}

vector<FastSubst> FastQueryACUnify::solveAC(UnifEq ueq) {
  FastFunc f = getFunc(ueq.t1);
  FastTerm unityElement = -1; // ?? getFunTerm(f->unityElement, {});
  function<void(FastTerm, map<FastTerm, int>&, map<FastTerm, FastTerm>&)> getCoeffs;
  getCoeffs = [&](FastTerm t, map<FastTerm, int> &M, map<FastTerm, FastTerm> &constToVar) {
    if(isVariable(t)) {
      ++M[t];
      return;
    }
    if(!getArity(t) || getFunc(t) != f) {
      if(!constToVar.count(t)) {
        constToVar[t] = -1; // ?? getVarTerm(createFreshVariable((Sort*) getSort("State")));
      }
      ++M[constToVar[t]];
      return;
    }
    auto argst = args(t);
    int n = getArity(t);
    for (int i = 0; i < n; ++i) {
      getCoeffs(argst[i], M, constToVar);
    }
  };
  map<FastTerm, int> l, r;
  map<FastTerm, FastTerm> constToVar;
  getCoeffs(ueq.t1, l, constToVar);
  getCoeffs(ueq.t2, r, constToVar);
  delSameCoeffs(l, r);
  if (!l.size() || !r.size()) {
    if (l.size() != r.size()) {
      return {};
    }
    return vector<FastSubst>{FastSubst()};
  }
  vector<int> a = fromMapToVector(l);
  vector<int> b = fromMapToVector(r);
  LDEGraphAlg graphAlg(a, b);
  vector<pair<vector<int>, vector<int>>> result = graphAlg.solve();
  if (!result.size()) {
    return {};
  }
  vector<FastSubst> sigma;
  for (const auto &sol : result) {
    int index = 0;
    sigma.push_back(FastSubst());
    FastTerm z = -1; // ?getVarTerm(createFreshVariable((Sort*) getSort("State")));
    for (auto it : l) {
      sigma.back().addToSubst(it.first, createFuncWithSameVar(sol.first[index], z,f, unityElement));
      ++index;
    }
    index = 0;
    for (auto it : r) {
      sigma.back().addToSubst(it.first, createFuncWithSameVar(sol.second[index], z, f, unityElement));
      ++index;
    }
  }
  vector<vector<FastTerm>> sigmaImage(sigma.size(), vector<FastTerm>(l.size() + r.size()));
  for(int i = 0; i < (int)sigma.size(); ++i) {
    int index = 0;
    for(auto it : l) {
      sigmaImage[i][index] = sigma[i].range(it.first);
      ++index;
    }
    for(auto it : r) {
      sigmaImage[i][index] = sigma[i].range(it.first);
      ++index;
    }
  }
  auto checkConstConstraints = [&](FastSubst &subst) -> bool {
    map<FastTerm, FastTerm> constSubst;
    for (const auto &it : constToVar) {
      FastVar var = it.second;
      if (subst.inDomain(var)) {
        FastTerm t = subst.range(var);
        if(isFuncTerm(t) && t != it.second) {
          return false;
        }
        if(isVariable(t)) {
          if(constSubst.count(t) && constSubst[t] != it.first) {
            return false;
          }
          constSubst[t] = it.first;
        }
      }
    }
    for (const auto &it : constSubst) {
      subst.composeWith(it.first, it.second);
    }
    return true;
  };
  auto checkMask = [&](int mask) -> bool {
    int n = sigma.size();
    int m = sigmaImage[0].size();
    int used = 0;
    const int allBits = (1 << m) - 1;
    for (int i = 0; i < n; ++i) {
      if (mask & (1 << i)) {
        continue;
      }
      for (int j = 0; j < m; ++j) {
        if (used & (1 << j)) {
          continue;
        }
        FastTerm aux = sigmaImage[i][j];
        if (isVariable(aux) || aux != unityElement) {
          used |= 1 << j;
          if (used == allBits) {
            return true;
          }
        }
      }
    }
    return false;
  };
  auto getSubstFromMask = [&](const int &mask, FastSubst &subst) -> bool {
    int n = sigma.size();
    int m = sigmaImage[0].size();
    vector<FastTerm> ans(m);
    for (int i = 0; i < n; ++i) {
      if (mask & (1 << i)) {
        continue;
      }
      for (int j = 0; j < m; ++j) {
        FastTerm aux = sigmaImage[i][j];
        if (isVariable(aux) || aux != unityElement) {
          FastTerm p[2] = {ans[j], aux};
          ans[j] = ans[j] ? newFuncTerm(f, p) : aux;
        }
      }
    }
    int index = 0;
    for (auto it : l) {
      subst.addToSubst(it.first, ans[index]);
      ++index;
    }
    for (auto it : r) {
      subst.addToSubst(it.first, ans[index]);
      ++index;
    }
    return checkConstConstraints(subst);
  };
  vector<FastSubst> minSubstSet;
  minSubstSet.push_back(FastSubst());
  if (!checkMask(0) || !getSubstFromMask(0, minSubstSet.back())) {
    minSubstSet.pop_back();
  }
  const int upperMask = 1 << sigma.size();
  for (int mask = 1; mask < upperMask; ++mask) {
    if (checkMask(mask)) {
      FastSubst subst;
      if (getSubstFromMask(mask, subst)) {
        minSubstSet.push_back(subst);
      }
    }
  }
  if (!minSubstSet.size()) {
    return {};
  }
  map<FastTerm, FastTerm> invConstToVar;
  for(const auto &it : constToVar) {
    invConstToVar[it.second] = it.first;
  }
  for (auto &subst : minSubstSet) {
    FastSubst normalizedSubst;
    int n = subst.count;
    for (int i = 0; i < n; i += 2) {
      if (!invConstToVar.count(subst.data[i]) && l.count(subst.data[i]) + r.count(subst.data[i]) > 0) {
        normalizedSubst.composeWith(subst.data[i], subst.data[i + 1]);
      }
    }
    subst = move(normalizedSubst);
  }
  return minSubstSet;
  
}

vector<FastSubst> FastQueryACUnify::solve(UnifEqSystem ues) {
  vector<FastSubst> minSubstSet;
  queue<pair<UnifEqSystem, FastSubst>> q;
  for (q.push(make_pair(ues, FastSubst())); !q.empty(); q.pop()) {
    ues = move(q.front().first);
    FastSubst subst = move(q.front().second);
    bool toAdd = true;
    while (ues.size() && toAdd) {
      UnifEq eq = ues.back();
      if (eq.t1 == eq.t2) {
        ues.pop_back();
        continue;
      }
      if (isVariable(eq.t1)) {
        if (isFuncTerm(eq.t2) && occurs(eq.t1, eq.t2)) {
          toAdd = false;
          break;
        }
        subst.composeWith(eq.t1, eq.t2);
        ues.pop_back();
        for (auto &it : ues) {
          it.t1 = applyUnitySubst(it.t1, eq.t1, eq.t2);
          it.t2 = applyUnitySubst(it.t2, eq.t1, eq.t2);
        }
        continue;
      }
      if (isFuncTerm(eq.t1) && isFuncTerm(eq.t2)) {
        auto func1 = getFunc(eq.t1);
        auto func2 = getFunc(eq.t2);
        if (func1 != func2) {
          toAdd = false;
          break;
        }
        if (1/*func1->isAssociative*/ && 1/*func1->isCommutative*/) {
          vector<FastSubst> sols = solveAC(eq);
          ues.pop_back();
          for (auto &sol : sols) {
            q.push(make_pair(UnifEqSystem(sol, ues), subst));
          }
          toAdd = false;
          break;
        }
        UnifEq toDecomp = move(eq);
        ues.pop_back();
        ues.decomp(toDecomp.t1, toDecomp.t2);
      }
    }
    if (toAdd) {
      minSubstSet.push_back(move(subst));
    }
  }
  return minSubstSet;
}

void FastQueryACUnify::execute() {
  char buff[1000];
  printTerm(t1, buff, 1000);
  cout << "AC-Unifying " << buff << " and ";
  printTerm(t2, buff, 1000);
  cout << buff << endl;
  vector<FastSubst> minSubstSet = solve(UnifEqSystem(t1, t2));
  if (!minSubstSet.size()) {
    cout << "No unification." << endl;
    return;
  }
  for (auto &subst : minSubstSet) {
    printSubst(subst, buff, 1000);
    cout << buff << endl;
  }
}