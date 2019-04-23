#include <iostream>
#include "queryacunify.h"
#include "parse.h"
#include "factories.h"
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

QueryACUnify::QueryACUnify(Term *t1, Term *t2) {
  if (t1->isFunTerm && t2->isVarTerm) {
    swap(t1, t2);
  }
  this->t1 = t1;
  this->t2 = t2;
}
  
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

vector<Substitution> QueryACUnify::solve() {
  if (t1 == t2) {
    return vector<Substitution> ({Substitution()});
  }
  if (t1->isFunTerm && t2->isFunTerm) {
    if (t1->getAsFunTerm()->function != t2->getAsFunTerm()->function) {
      return {};
    }
    vector<Substitution> minSubstSet;
    return minSubstSet;
  }
  if (t2->isFunTerm && t2->hasVariable(t1->getAsVarTerm()->variable)) {
    return {};
  }
  return vector<Substitution> ({Substitution(t1->getAsVarTerm()->variable, t2)});
}

void QueryACUnify::execute() {
  cout << "AC-Unifying " << t1->toString() << " and " << t2->toString() << endl;
  vector<Substitution> minSubstSet = solve();
  for (auto &subst : minSubstSet) {
    cout << subst.toString() << endl;
  }
}