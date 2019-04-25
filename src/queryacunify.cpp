#include <iostream>
#include <queue>
#include "queryacunify.h"
#include "parse.h"
#include "varterm.h"
#include "funterm.h"
#include "unifeq.h"
#include "unifeqsystem.h"

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
  if (t1->isFunTerm && t2->isVarTerm) {
    swap(t1, t2);
  }
}

bool QueryACUnify::solvedForm(const UnifEqSystem &ues) {
  for (const auto &eq1 : ues) {
    if (eq1.t1->isFunTerm) {
      return false;
    }
    for (const auto &eq2 : ues) {
      if (eq2.t2->hasVariable(eq1.t1->getAsVarTerm()->variable)) {
        return false;
      }
    }
  }
  return true;
}

Substitution QueryACUnify::getSubstFromSolvedForm(const UnifEqSystem &ues) {
  Substitution subst;
  for (const auto &eq : ues) {
    subst.force(eq.t1->getAsVarTerm()->variable, eq.t2);
  }
  return subst;
}

vector<Substitution> QueryACUnify::solve(UnifEqSystem ues) {
  vector<Substitution> substSet;
  queue<UnifEqSystem> q;
  for (q.push(move(ues)); !q.empty(); q.pop()) {
    ues = move(q.front());
    if (solvedForm(ues)) {
      substSet.push_back(getSubstFromSolvedForm(ues));
      continue;
    }
    Substitution subst;
    while (ues.size()) {
      UnifEq &eq = ues.back();
      if (eq.t1 == eq.t2) {
        ues.pop_back();
        continue;
      }
      if (eq.t1->isVarTerm) {
        if (eq.t2->isFunTerm && eq.t2->hasVariable(eq.t1->getAsVarTerm()->variable)) {
          break;
        }
        subst.force(eq.t1->getAsVarTerm()->variable, eq.t2);
        Substitution aux(eq.t1->getAsVarTerm()->variable, eq.t2);
        ues.pop_back();
        for (auto &it : ues) {
          it.t1 = it.t1->substitute(aux);
          it.t2 = it.t2->substitute(aux);
        }
        continue;
      }
      if (eq.t1->isFunTerm && eq.t2->isFunTerm) {
        if (eq.t1->getAsFunTerm()->function != eq.t2->getAsFunTerm()->function) {
          break;
        }
        UnifEq toDecomp = move(eq);
        ues.pop_back();
        ues.decomp(toDecomp.t1->getAsFunTerm(), toDecomp.t2->getAsFunTerm());
      }
    }
    if (!ues.size()) {
      substSet.push_back(subst);
    }
  }
  return substSet;
}

void QueryACUnify::execute() {
  cout << "AC-Unifying " << t1->toString() << " and " << t2->toString() << endl;
  vector<Substitution> minSubstSet = solve(UnifEqSystem(t1, t2));
  if (!minSubstSet.size()) {
    cout << "No unification." << endl;
    return;
  }
  for (auto &subst : minSubstSet) {
    cout << subst.toString() << endl;
  }
}