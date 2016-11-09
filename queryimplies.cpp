#include "queryimplies.h"
#include "parse.h"
#include "factories.h"
#include "z3driver.h"
#include "log.h"
#include <iostream>
#include <string>
#include <map>
#include <cassert>

using namespace std;

QueryImplies::QueryImplies() :
  ct1(0, 0),
  ct2(0, 0)
{
}

Query *QueryImplies::create()
{
  return new QueryImplies();
}
  
void QueryImplies::parse(std::string &s, int &w)
{
  matchString(s, w, "implies");
  skipWhiteSpace(s, w);
  ct1 = parseConstrainedTerm(s, w);
  skipWhiteSpace(s, w);
  ct2 = parseConstrainedTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");
}

void QueryImplies::execute()
{
  Substitution subst;
  Term *constraint = 0;
  assert(ct1->constraint);
  assert(ct2->constraint);
  cout << "Testing implication between " << ct1.toString() << " and " << ct2.toString() << endl;
  vector<Variable *> v1 = ct1.vars();
  vector<Variable *> v2 = ct2.vars();
  vector<Variable *> vclosure;
  for (int i = 0; i < (int)v1.size(); ++i) {
    bool ok = true;
    for (int j = 0; j < (int)v2.size(); ++j) {
      if (v1[i] == v2[j]) {
	ok = false;
	break;
      }
    }
    if (ok) {
      vclosure.push_back(v1[i]);
    }
  }
  Log(DEBUG5) << "Unifying " << ct1.term->toString() << " and " << ct2.term->toString() << endl;
  if (ct1.term->unifyModuloTheories(ct2.term, subst, constraint)) {
    Log(DEBUG5) << "Unification succeeded. Result:" << endl;
    Log(DEBUG5) << "Substitution = " << subst.toString() << endl;
    Log(DEBUG5) << "Constraint = " << constraint->toString() << endl;
    Term *c1 = ct1.constraint->substitute(subst);
    Term *c2 = ct2.constraint->substitute(subst);
    Term *constraintToCheck = introduceExists(bImplies(c1, bAnd(constraint, c2)), vclosure);
    Log(DEBUG5) << "Checking validity of " << constraintToCheck->toSmtString() << endl;
    Z3Result result = isSatisfiable(bNot(constraintToCheck));

    cout << "Implication results: ";
    if (result == sat) {
      cout << "not valid." << endl;
    } else if (result == unsat) {
      cout << "valid." << endl;
    } else if (result == unknown) {
      cout << "inconclusive (smt solver returned unknown)." << endl;
    } else {
      cout << "inconclusive (smt solver did not properly terminate)." << endl;
    }
  } else {
      cout << "not valid (no unification)." << endl;
  }
}
