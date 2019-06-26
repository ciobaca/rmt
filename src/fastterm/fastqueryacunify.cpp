#include <iostream>
#include <queue>
#include <functional>
#include "fastqueryacunify.h"
#include "unifeq.h"
#include "fastterm.h"
#include "unifeqsystem.h"
#include "ldegraphalg.h"

using namespace std;

FastQueryACUnify::FastQueryACUnify(FastTerm t1, FastTerm t2) : t1(t1), t2(t2) {}

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
  FastFunc uElemConst = getUnityElement(f);
  FastTerm uElemTerm = newFuncTerm(uElemConst, nullptr);
  function<void(FastTerm, map<FastTerm, int>&, map<FastTerm, FastTerm>&)> getCoeffs;
  getCoeffs = [&](FastTerm t, map<FastTerm, int> &M, map<FastTerm, FastTerm> &constToVar) {
    if(isVariable(t)) {
      ++M[t];
      return;
    }
    if(!getArity(getFunc(t)) || getFunc(t) != f) { 
      if(!constToVar.count(t)) {
        constToVar[t] = createFreshVariable(fastStateSort());
      }
      ++M[constToVar[t]];
      return;
    }
    int n = getArity(getFunc(t));
    for (int i = 0; i < n; ++i) {
      getCoeffs(getArg(t, i), M, constToVar);
    }
  };
  map<FastTerm, int> l, r;
  map<FastTerm, FastTerm> constToVar;
  getCoeffs(ueq.t1, l, constToVar);
  getCoeffs(ueq.t2, r, constToVar);
  delSameCoeffs(l, r);
  if (!l.size() || !r.size()) {
    if (l.size() != r.size()) {
      if (uElemConst == MISSING_UELEM) {
        return {};
      }
      FastSubst subst;
      for (auto it : l) {
        subst.addToSubst(it.first, uElemTerm);
      }
      for (auto it : r) {
        subst.addToSubst(it.first, uElemTerm);
      }
      return vector<FastSubst>{subst};
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
    FastTerm z = createFreshVariable(fastStateSort());
    for (auto it : l) {
      sigma.back().addToSubst(it.first, createFuncWithSameVar(sol.first[index], z, f, uElemTerm));
      ++index;
    }
    index = 0;
    for (auto it : r) {
      sigma.back().addToSubst(it.first, createFuncWithSameVar(sol.second[index], z, f, uElemTerm));
      ++index;
    }
  }
  vector<vector<FastTerm>> sigmaImage(sigma.size(), vector<FastTerm>(l.size() + r.size()));
  for(int i = 0; i < (int)sigma.size(); ++i) {
    int index = 0;
    for(auto it : l) {
      sigmaImage[i][index] = sigma[i].image(it.first);
      ++index;
    }
    for(auto it : r) {
      sigmaImage[i][index] = sigma[i].image(it.first);
      ++index;
    }
  }
  auto checkConstConstraints = [&](FastSubst &subst) -> bool {
    map<FastTerm, FastTerm> constSubst;
    for (const auto &it : constToVar) {
      FastVar var = it.second;
      if (subst.inDomain(var)) {
        FastTerm t = subst.image(var);
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
        if (isVariable(aux) || aux != uElemTerm) {
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
    vector<FastTerm> ans(m, MISSING_UELEM);
    for (int i = 0; i < n; ++i) {
      if (mask & (1 << i)) {
        continue;
      }
      for (int j = 0; j < m; ++j) {
        FastTerm aux = sigmaImage[i][j];
        if (isVariable(aux) || aux != uElemTerm) {
          FastTerm p[2] = {ans[j], aux};
          ans[j] = (ans[j] != MISSING_UELEM ? newFuncTerm(f, p) : aux);
        }
      }
    }
    if (uElemConst != MISSING_UELEM) {
      for (auto &it : ans) {
        if (it == MISSING_UELEM) {
          it = uElemTerm;
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
  const int upperMask = 1 << sigma.size();
  for (int mask = 0; mask < upperMask; ++mask) {
    if (uElemConst != MISSING_UELEM || checkMask(mask)) {
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
    subst = normalizedSubst;
  }
  return minSubstSet;
}

#include <iostream>

vector<FastSubst> FastQueryACUnify::solve() {
  UnifEqSystem ues(t1, t2);
  vector<FastSubst> minSubstSet;
  queue<pair<UnifEqSystem, FastSubst>> q;
  for (q.push(make_pair(ues, FastSubst())); !q.empty(); q.pop()) {
    ues = q.front().first;
    FastSubst subst = q.front().second;
    bool toAdd = true;
    while (ues.size() && toAdd) {
      UnifEq eq = ues.back();
      if (eq_term(eq.t1, eq.t2)) {
        ues.pop_back();
        continue;
      }
      FastSort s1 = getSort(eq.t1);
      FastSort s2 = getSort(eq.t2);
      if (!isSubSortTransitive(s1, s2) &&
	  !isSubSortTransitive(s2, s1)) {
	toAdd = false;
	break;
      }
      if (isVariable(eq.t1)) {
        if (isFuncTerm(eq.t2) && occurs(eq.t1, eq.t2)) {
          FastFunc uElem = getUnityElement(getFunc(eq.t2));
          if (uElem != MISSING_UELEM) {
	    FastTerm uElemTerm = newFuncTerm(uElem, nullptr);
	    ues.pop_back();
            auto arg1 = args(eq.t2)[0];
            auto arg2 = args(eq.t2)[1];
            UnifEqSystem nues(ues);
            nues.addEq(UnifEq(arg1, uElemTerm));
            nues.addEq(UnifEq(arg2, eq.t1), true);
            q.push(make_pair(nues, subst));
            if (arg1 != arg2) {
              nues = UnifEqSystem(ues);
              nues.addEq(UnifEq(arg1, eq.t1));
              nues.addEq(UnifEq(arg2, uElemTerm), true);
              q.push(make_pair(nues, subst));
            }
          }
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
          ues.pop_back();
          if (getUnityElement(func1) != MISSING_UELEM) {
            FastFunc uElem = getUnityElement(func1);
            FastTerm uElemTerm = newFuncTerm(uElem, nullptr);
            auto arg1 = getArg(eq.t1, 0);
            auto arg2 = getArg(eq.t1, 1);
            UnifEqSystem nues(ues);
            nues.addEq(UnifEq(arg1, uElemTerm));
            nues.addEq(UnifEq(arg2, eq.t2), true);
            q.push(make_pair(nues, subst));
            nues = UnifEqSystem(ues);
            nues.addEq(UnifEq(arg1, eq.t2));
            nues.addEq(UnifEq(arg2, uElemTerm), true);
            q.push(make_pair(nues, subst));
          }
          if (getUnityElement(func2) != MISSING_UELEM) {
            FastFunc uElem = getUnityElement(func2);
            FastTerm uElemTerm = newFuncTerm(uElem, nullptr);
            auto arg1 = getArg(eq.t2, 0);
            auto arg2 = getArg(eq.t2, 1);
            UnifEqSystem nues(ues);
            nues.addEq(UnifEq(arg1, uElemTerm));
            nues.addEq(UnifEq(arg2, eq.t1), true);
            q.push(make_pair(nues, subst));
            nues = UnifEqSystem(ues);
            nues.addEq(UnifEq(arg1, eq.t1));
            nues.addEq(UnifEq(arg2, uElemTerm), true);
            q.push(make_pair(nues, subst));
          }
          toAdd = false;
          break;
        }
        if (isFuncAC(func1)) {
          vector<FastSubst> sols = solveAC(eq);
          ues.pop_back();
          for (auto &sol : sols) {
            q.push(make_pair(UnifEqSystem(sol, ues), subst));
          }
          toAdd = false;
          break;
        }
        ues.pop_back();
        ues.decomp(eq.t1, eq.t2);
      }
    }
    if (toAdd) {
      minSubstSet.push_back(subst);
    }
  }
  return minSubstSet;
}
