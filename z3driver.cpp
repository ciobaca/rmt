#include <cstdlib>
#include <sstream>
#include <cassert>
#include <cstdio>
#include <iostream>
#include "z3driver.h"
#include "variable.h"
#include "log.h"
#include "term.h"
#include "sort.h"
#include "factories.h"
#include <string.h>

using namespace std;

string smt_prelude;

string callz3(string s)
{
  FILE *fout = fopen("z3_temp.input", "w");
  fprintf(fout, "%s\n", smt_prelude.c_str());
  fprintf(fout, "%s", s.c_str());
  fclose(fout);
  system("z3 -T:1 -in < z3_temp.input > z3_temp.output");
  FILE *fin = fopen("z3_temp.output", "r");
  char z3_result[1024];
  // TODO: fix fixed length
  fscanf(fin, "%s", z3_result);
  fclose(fin);
  return z3_result;
}

void Z3Theory::addVariable(Variable *var)
{
  variables.push_back(var);
}

void Z3Theory::addEqualityConstraint(Term *left, Term *right)
{
  Function *EqualsFun = getFunction("bequals");
  vector<Term *> sides;
  sides.push_back(left);
  sides.push_back(right);
  addConstraint(getFunTerm(EqualsFun, sides));
}

void Z3Theory::addConstraint(Term *constraint)
{
  constraints.push_back(constraint);
}

Z3Result Z3Theory::isSatisfiable()
{
  ostringstream oss;
  for (int i = 0; i < variables.size(); ++i) {
    assert(variables[i]->sort->hasInterpretation);
    oss << "(declare-const " << variables[i]->name << " " << variables[i]->sort->interpretation << ")" << endl;
  }
  for (int i = 0; i < constraints.size(); ++i) {
    oss << "(assert " << constraints[i]->toSmtString() << ")" << endl;
  }
  oss << "(check-sat)" << endl;
  string z3string = oss.str();
  Log(LOGSAT) << "Sending the following to Z3:" << endl << z3string;
  string result = callz3(z3string);
  if (result == "sat") {
    Log(LOGSAT) << "Result is SAT" << endl << z3string;
    return sat;
  } else if (result == "unsat") {
    Log(LOGSAT) << "Result is UNSAT" << endl << z3string;
    return unsat;
  } else if (result == "unknown") {
    Log(LOGSAT) << "Result is UNKNOWN" << endl << z3string;
    return unknown;
  } else if (result == "timeout") {
    Log(LOGSAT) << "Result is timeout" << endl << z3string;
    return unknown;
  } else {
    Log(ERROR) << "Internal error - Z3 did not return an expected satisfiability value." << endl << z3string;
    Log(ERROR) << "Tried to smt the following constraints:" << endl;
    for (int i = 0; i < constraints.size(); ++i) {
      Log(ERROR) << constraints[i]->toString() << endl;
    }
    fprintf(stderr, "Cannot interpret result returned by Z3: \"%s\".\n", result.c_str());
    assert(0);
  }
}

Z3Result isSatisfiable(Term *constraint)
{
  Log(LOGSAT) << "Testing satisfiability of " << constraint->toString() << endl;
  Z3Theory theory;
  vector<Variable *> interpretedVariables = getInterpretedVariables();
  for (int i = 0; i < interpretedVariables.size(); ++i) {
    theory.addVariable(interpretedVariables[i]);
  }
  theory.addConstraint(constraint);
  return theory.isSatisfiable();
}
