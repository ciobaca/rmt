#include "queryacuunify.h"
#include "parse.h"
#include "factories.h"
#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <vector>
#include <tuple>
#include <chrono>
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
using namespace std::chrono;

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

void QueryACUUnify::execute() {
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

  cout << "ACU-Unifying " << t1->toString() << " and " << t2->toString() << endl;
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

  Substitution finalSubst;
  for (auto it : l) {
    Term *ans = getFunTerm(getFunction("e"), {});
    for (auto subst : sigma) {
      ans = getFunTerm(f, {ans, subst.image(it.first->getAsVarTerm()->variable)});
    }
    finalSubst.add(it.first->getAsVarTerm()->variable, ans);
  }
  for (auto it : r) {
    Term *ans = getFunTerm(getFunction("e"), {});
    for (auto subst : sigma) {
      ans = getFunTerm(f, {ans, subst.image(it.first->getAsVarTerm()->variable)});
    }
    finalSubst.add(it.first->getAsVarTerm()->variable, ans);
  }

  cout << finalSubst.toString() << endl;
}

/*
void QueryACUUnify::execute() {
  cout << "ACU-Unifying " << t1->toString() << " and " << t2->toString() << endl;

//  vector<int> a = {2, 2, 2, 3, 3, 3};
//  vector<int> b = {2, 2, 2, 3, 3, 3};
  vector<int> a = {1, 3, 5, 7, 9, 11};
  vector<int> b = {2, 4, 6, 8, 10, 12};

  LDELexAlg lexAlg0(a, b, 0, 0);
  LDELexAlg lexAlg1(a, b, 0, 1);
  LDELexAlg lexAlg2(a, b, 0, 2);

  LDECompAlg compAlg0(a, b, 0, 0);
  LDECompAlg compAlg1(a, b, 0, 1);

  LDEGraphAlg graphAlg(a, b, 0);

  LDESlopesAlg slopesAlg(a, b, 0);

  vector<pair<vector<int>, vector<int>>> result;
  high_resolution_clock::time_point t1, t2;

  t1 = high_resolution_clock::now();
  //result = lexAlg0.solve();
  t2 = high_resolution_clock::now();
  cout << result.size() << ' ' << duration_cast<microseconds>(t2 - t1).count() / 1e6 << endl;

  t1 = high_resolution_clock::now();
  //result = lexAlg1.solve();
  t2 = high_resolution_clock::now();
  cout << result.size() << ' ' << duration_cast<microseconds>(t2 - t1).count() / 1e6 << endl;

  t1 = high_resolution_clock::now();
  //result = lexAlg2.solve();
  t2 = high_resolution_clock::now();
  cout << result.size() << ' ' << duration_cast<microseconds>(t2 - t1).count() / 1e6 << endl;

  cout << "Slopes\n";
  t1 = high_resolution_clock::now();
  result = slopesAlg.solve();
  t2 = high_resolution_clock::now();
  cout << result.size() << ' ' << duration_cast<microseconds>(t2 - t1).count() / 1e6 << endl;
/*  for (auto it : result) {
    //cout << it.first[0] << ' ' << it.first[1] << ' ' << it.first[2] << ' ' << it.second[0] << ' ' << it.second[1] << ' ' << it.second[2] << ' ' << it.second[3] << '\n';
    cout << 1 * it.first[0] + 2 * it.first[1] + 3 * it.first[2] - 7 * it.second[0] - 5 * it.second[1] - 6 * it.second[2] - 4 * it.second[3] << '\n';
 }*//*
  vector<pair<vector<int>, vector<int>>> aux = result;
/*  t1 = high_resolution_clock::now();
  result = compAlg0.solve();
  t2 = high_resolution_clock::now();
  cout << result.size() << ' ' << duration_cast<microseconds>(t2 - t1).count() / 1e6 << endl;

  t1 = high_resolution_clock::now();
  result = compAlg1.solve();
  t2 = high_resolution_clock::now();
  cout << result.size() << ' ' << duration_cast<microseconds>(t2 - t1).count() / 1e6 << endl;
  */  /*
  t1 = high_resolution_clock::now();
  result = graphAlg.solve();
  t2 = high_resolution_clock::now();
  cout << result.size() << ' ' << duration_cast<microseconds>(t2 - t1).count() / 1e6 << endl;
  sort(result.begin(), result.end());
  sort(aux.begin(), aux.end());
  if (aux == result) cout << "DA, SUNT EGALE\n";
/*  for(int i = 0; i < (int)aux.size(); ++i) {
    auto it = aux[i];
    cout << it.first[0] << ' ' << it.first[1] << ' ' << it.first[2] << ' ' << it.second[0] << ' ' << it.second[1] << ' ' << it.second[2] << ' ' << it.second[3] << '\n';
    it = result[i];
    cout << it.first[0] << ' ' << it.first[1] << ' ' << it.first[2] << ' ' << it.second[0] << ' ' << it.second[1] << ' ' << it.second[2] << ' ' << it.second[3] << '\n';
    cin.get();
  }*/ 
/*}*/