#include <iostream>
#include <queue>
#include "queryacunify.h"
#include "parse.h"
#include "varterm.h"
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

vector<Substitution> QueryACUnify::solve(UnifEqSystem ues) {
  vector<Substitution> substSet;
  queue<UnifEqSystem> q;
  for (q.push(move(ues)); !q.empty(); q.pop()) {
    ues = move(q.front());
    Substitution subst;
    while (ues.size()) {
      
    }
  }
  return substSet;
}

void QueryACUnify::execute() {
  cout << "AC-Unifying " << t1->toString() << " and " << t2->toString() << endl;
  vector<Substitution> minSubstSet = solve(UnifEqSystem(t1, t2));
  for (auto &subst : minSubstSet) {
    cout << subst.toString() << endl;
  }
}