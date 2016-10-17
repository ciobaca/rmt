#include "querysmtsatisfiability.h"
#include "parse.h"
#include "z3driver.h"
#include <string>
#include <iostream>
#include <map>
#include <cassert>

using namespace std;

QuerySmtSatisfiability::QuerySmtSatisfiability()
{
}

Query *QuerySmtSatisfiability::create()
{
  return new QuerySmtSatisfiability();
}
  
void QuerySmtSatisfiability::parse(std::string &s, int &w)
{
  matchString(s, w, "satisfiability");
  skipWhiteSpace(s, w);
  constraint = parseTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");
}

void QuerySmtSatisfiability::execute()
{
  Z3Result result = isSatisfiable(constraint);
  if (result == sat) {
    cout << "The constraint " << constraint->toSmtString() << " is SAT." << endl;
  } else if (result == unsat) {
    cout << "The constraint " << constraint->toSmtString() << " is UNSAT." << endl;
  } else if (result == unknown) {
    cout << "Satisfiability check of constraint " << constraint->toSmtString() << " is not conclusive." << endl;
  } else {
    assert(0);
  }
}
