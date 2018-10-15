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
#include "helper.h"
#include <string>

#include <z3.h>

Z3_context z3context;
Z3_solver z3solver;

void z3_error_handler(Z3_context context, Z3_error_code error)
{
  Z3_string string_error = Z3_get_error_msg(context, error);
  abortWithMessage(string("Z3 returned non OK error code (") + string_error + ".");
}

void startz3api()
{
  Z3_config z3config = Z3_mk_config();
  Z3_set_param_value(z3config, "timeout", "200");
  Z3_set_param_value(z3config, "auto_config", "true");
  z3context = Z3_mk_context(z3config);
  Z3_set_error_handler(z3context, z3_error_handler);

  Z3_sort z3BoolSort = Z3_mk_bool_sort(z3context);
  Z3_sort z3IntSort = Z3_mk_int_sort(z3context);

  Z3_ast z3True = Z3_mk_true(z3context);
  Z3_ast z3False = Z3_mk_false(z3context);

  z3solver = Z3_mk_solver(z3context);
  Z3_solver_assert(z3context, z3solver, z3True);
  if (Z3_solver_check(z3context, z3solver) != Z3_L_TRUE) {
    abortWithMessage("Z3 said true is unsatisfiable or undef.");
  }
  Z3_solver_assert(z3context, z3solver, z3False);
  if (Z3_solver_check(z3context, z3solver) != Z3_L_FALSE) {
    abortWithMessage("Z3 said true /\\ false is satisfiable or undef.");
  }
}

using namespace std;

string smt_prelude;

string prover = "z3 -T:5 -in <";
//string prover = "cvc4 --lang smtlib2";

string callz3(string s)
{
  FILE *fout = fopen("z3_temp.input", "w");
  fprintf(fout, "%s\n", smt_prelude.c_str());
  fprintf(fout, "%s", s.c_str());
  fclose(fout);
  system((prover + " z3_temp.input > z3_temp.output").c_str());
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
  for (int i = 0; i < (int)variables.size(); ++i) {
    assert(variables[i]->sort->hasInterpretation);
    oss << "(declare-const " << variables[i]->name << " " << variables[i]->sort->interpretation << ")" << endl;
  }
  for (int i = 0; i < (int)constraints.size(); ++i) {
    oss << "(assert " << constraints[i]->toSmtString() << ")" << endl;
  }
  oss << "(check-sat)" << endl;
  string z3string = oss.str();
  Log(LOGSAT) << "Sending the following to Z3:" << endl << z3string;
  string result = callz3(z3string);
  if (result == "sat") {
    Log(LOGSAT) << "Result is SAT" << endl;
    return sat;
  } else if (result == "unsat") {
    Log(LOGSAT) << "Result is UNSAT" << endl;
    return unsat;
  } else if (result == "unknown") {
    Log(LOGSAT) << "Result is UNKNOWN" << endl;
    Log(WARNING) << "Result is UNKNOWN" << endl;
    return unknown;
  } else if (result == "timeout") {
    Log(LOGSAT) << "Result is timeout" << endl;
    Log(WARNING) << "Result is timeout" << endl;
    return unknown;
  } else {
    Log(ERROR) << "Internal error - Z3 did not return an expected satisfiability value." << endl << z3string;
    Log(ERROR) << "Tried to smt the following constraints:" << endl;
    for (int i = 0; i < (int)constraints.size(); ++i) {
      Log(ERROR) << constraints[i]->toString() << endl;
    }
    fprintf(stderr, "Cannot interpret result returned by Z3: \"%s\".\n", result.c_str());
    assert(0);
	return unknown;
  }
}

Z3Result isSatisfiable(Term *constraint)
{
  Log(LOGSAT) << "Testing satisfiability of " << constraint->toString() << endl;
  Z3Theory theory;
  vector<Variable *> interpretedVariables = getInterpretedVariables();
  for (int i = 0; i < (int)interpretedVariables.size(); ++i) {
    theory.addVariable(interpretedVariables[i]);
  }
  theory.addConstraint(constraint);
  return theory.isSatisfiable();
}
