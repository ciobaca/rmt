#include <cstdlib>
#include <sstream>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <vector>
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

Z3_sort z3BoolSort;
Z3_sort z3IntSort;

using namespace std;

unsigned z3_symbol_count = 0;

Z3_ast z3_make_constant(Sort *sort)
{
  Z3_symbol symbol = Z3_mk_int_symbol(z3context, z3_symbol_count++);
  return Z3_mk_const(z3context, symbol, sort->interpretation);
}

Z3_sort z3_bool()
{
  return z3BoolSort;
}

Z3_sort z3_int()
{
  return z3IntSort;
}

Z3_ast z3_add(vector<Term *> args)
{
  assert(args.size() == 2);
  Z3_ast z3args[2];
  z3args[0] = args[0]->toSmt();
  z3args[1] = args[1]->toSmt();
  return Z3_mk_add(z3context, 2, z3args);
}

Z3_ast z3_mul(vector<Term *> args)
{
  assert(args.size() == 2);
  Z3_ast z3args[2];
  z3args[0] = args[0]->toSmt();
  z3args[1] = args[1]->toSmt();
  return Z3_mk_mul(z3context, 2, z3args);
}

Z3_ast z3_sub(vector<Term *> args)
{
  assert(args.size() == 2);
  Z3_ast z3args[2];
  z3args[0] = args[0]->toSmt();
  z3args[1] = args[1]->toSmt();
  return Z3_mk_sub(z3context, 2, z3args);
}

Z3_ast z3_div(vector<Term *> args)
{
  assert(args.size() == 2);
  return Z3_mk_div(z3context, args[0]->toSmt(), args[1]->toSmt());
}

Z3_ast z3_mod(vector<Term *> args)
{
  assert(args.size() == 2);
  return Z3_mk_mod(z3context, args[0]->toSmt(), args[1]->toSmt());
}

Z3_ast z3_le(vector<Term *> args)
{
  assert(args.size() == 2);
  return Z3_mk_le(z3context, args[0]->toSmt(), args[1]->toSmt());
}

Z3_ast z3_lt(vector<Term *> args)
{
  assert(args.size() == 2);
  return Z3_mk_lt(z3context, args[0]->toSmt(), args[1]->toSmt());
}

Z3_ast z3_eq(vector<Term *> args)
{
  assert(args.size() == 2);
  return Z3_mk_eq(z3context, args[0]->toSmt(), args[1]->toSmt());
}

Z3_ast z3_ct_0(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 0, z3IntSort);
}

Z3_ast z3_ct_1(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 1, z3IntSort);
}

Z3_ast z3_ct_2(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 2, z3IntSort);
}

Z3_ast z3_ct_3(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 3, z3IntSort);
}

Z3_ast z3_ct_4(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 4, z3IntSort);
}

Z3_ast z3_ct_5(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 5, z3IntSort);
}

Z3_ast z3_ct_6(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 6, z3IntSort);
}

Z3_ast z3_ct_7(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 7, z3IntSort);
}

Z3_ast z3_ct_8(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 8, z3IntSort);
}

Z3_ast z3_ct_9(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 9, z3IntSort);
}

Z3_ast z3_ct_10(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 10, z3IntSort);
}

Z3_ast z3_ct_11(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 11, z3IntSort);
}

Z3_ast z3_ct_12(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 12, z3IntSort);
}

Z3_ast z3_ct_13(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 13, z3IntSort);
}

Z3_ast z3_ct_14(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 14, z3IntSort);
}

Z3_ast z3_ct_15(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_int(z3context, 15, z3IntSort);
}

Z3_ast z3_true(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_true(z3context);
}

Z3_ast z3_false(vector<Term *> args)
{
  assert(args.size() == 0);
  return Z3_mk_false(z3context);
}


Z3_ast z3_not(vector<Term *> args)
{
  assert(args.size() == 1);
  return Z3_mk_not(z3context, args[0]->toSmt());
}
Z3_ast z3_and(vector<Term *> args)
{
  assert(args.size() == 2);
  Z3_ast z3args[2];
  z3args[0] = args[0]->toSmt();
  z3args[1] = args[1]->toSmt();
  return Z3_mk_and(z3context, 2, z3args);
}

Z3_ast z3_or(vector<Term *> args)
{
  assert(args.size() == 2);
  Z3_ast z3args[2];
  z3args[0] = args[0]->toSmt();
  z3args[1] = args[1]->toSmt();
  return Z3_mk_or(z3context, 2, z3args);
}

Z3_ast z3_iff(vector<Term *> args)
{
  assert(args.size() == 2);
  return Z3_mk_iff(z3context, args[0]->toSmt(), args[1]->toSmt());
}

Z3_ast z3_implies(vector<Term *> args)
{
  assert(args.size() == 2);
  return Z3_mk_implies(z3context, args[0]->toSmt(), args[1]->toSmt());
}

string smt_prelude;

string prover = "z3 -T:5 -in <";
//string prover = "cvc4 --lang smtlib2";

void z3_error_handler(Z3_context context, Z3_error_code error)
{
  Z3_string string_error = Z3_get_error_msg(context, error);
  abortWithMessage(string("Z3 returned non OK error code (") + string_error + ".");
}

void parse_z3_prelude(string prelude)
{
  smt_prelude = prelude;
  //  Z3_parse_smtlib2_string()
  /*
Z3_ast_vector Z3_API Z3_parse_smtlib2_string 	( 	Z3_context  	c,
		Z3_string  	str,
		unsigned  	num_sorts,
		Z3_symbol const  	sort_names[],
		Z3_sort const  	sorts[],
		unsigned  	num_decls,
		Z3_symbol const  	decl_names[],
		Z3_func_decl const  	decls[] 
		) */
  /*Z3_ast_vector conj = *///Z3_parse_smtlib2_string(z3context, prelude.c_str(),
  //		       0, 0, 0, 0, 0, 0);
}

void start_z3_api()
{
  Z3_config z3config = Z3_mk_config();
  Z3_set_param_value(z3config, "timeout", "200");
  Z3_set_param_value(z3config, "auto_config", "true");
  z3context = Z3_mk_context(z3config);
  Z3_set_error_handler(z3context, z3_error_handler);

  z3BoolSort = Z3_mk_bool_sort(z3context);
  z3IntSort = Z3_mk_int_sort(z3context);
  
  // Z3_ast z3True = Z3_mk_true(z3context);
  // Z3_ast z3False = Z3_mk_false(z3context);

  z3solver = Z3_mk_solver(z3context);
  // Z3_solver_assert(z3context, z3solver, z3True);
  // if (Z3_solver_check(z3context, z3solver) != Z3_L_TRUE) {
  //   abortWithMessage("Z3 said true is unsatisfiable or undef.");
  // }
  // Z3_solver_assert(z3context, z3solver, z3False);
  // if (Z3_solver_check(z3context, z3solver) != Z3_L_FALSE) {
  //   abortWithMessage("Z3 said true /\\ false is satisfiable or undef.");
  // }
}

void test_z3_api()
{
  Term *N = getVarTerm(createFreshVariable(getSort("Int")));
  vector<Term *> empty;
  Term *One = getFunTerm(getFunction("0"), empty);
  //  Term *B = getVarTerm(createFreshVariable(getSort("Bool")));
  Term *testTerm = bAnd(bTrue(), mle(One, N));
  Log(DEBUG7) << "Asserting " << testTerm->toString() << "." << endl;
  Z3_ast ast = testTerm->toSmt();
  Log(DEBUG7) << "Z3 Speak: asserting " << Z3_ast_to_string(z3context, ast) << "." << endl;
  Z3_solver_assert(z3context, z3solver, testTerm->toSmt());
  switch (Z3_solver_check(z3context, z3solver)) {
  case Z3_L_TRUE:
    cout << "satisfiable" << endl;
    break;
  case Z3_L_FALSE:
    cout << "unsatisfiable" << endl;
    break;
  case Z3_L_UNDEF:
    cout << "unknown" << endl;
    break;
  }
}

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
  for (int i = 0; i < (int)constraints.size(); ++i) {
    Z3_ast constraint = constraints[i]->toSmt();
    Log(DEBUG7) << "Asserting " << constraints[i]->toString() << "." << endl;
    Log(DEBUG7) << "Z3 Speak: asserting " << Z3_ast_to_string(z3context, constraint) << "." << endl;
    Z3_solver_assert(z3context, z3solver, constraint);
  }
  Log(DEBUG7) << "Calling solver." << endl;
  Z3_lbool result = Z3_solver_check(z3context, z3solver);
  if (result == Z3_L_TRUE) {
    Log(LOGSAT) << "Result is SAT" << endl;
    return sat;
  } else if (result == Z3_L_FALSE) {
    Log(LOGSAT) << "Result is UNSAT" << endl;
    return unsat;
  } else if (result == Z3_L_UNDEF) {
    Log(LOGSAT) << "Result is UNKNOWN (or timeout)" << endl;
    Log(WARNING) << "Result is UNKNOWN (or timeout)" << endl;
    return unknown;
  } else {
    assert(0);
  }
}

// Z3Result Z3Theory::isSatisfiable()
// {
//   ostringstream oss;
//   for (int i = 0; i < (int)variables.size(); ++i) {
//     assert(variables[i]->sort->hasInterpretation);
//     oss << "(declare-const " << variables[i]->name << " " << variables[i]->sort->interpretation << ")" << endl;
//   }
//   for (int i = 0; i < (int)constraints.size(); ++i) {
//     oss << "(assert " << constraints[i]->toSmtString() << ")" << endl;
//   }
//   oss << "(check-sat)" << endl;
//   string z3string = oss.str();
//   Log(LOGSAT) << "Sending the following to Z3:" << endl << z3string;
//   string result = callz3(z3string);
//   if (result == "sat") {
//     Log(LOGSAT) << "Result is SAT" << endl;
//     return sat;
//   } else if (result == "unsat") {
//     Log(LOGSAT) << "Result is UNSAT" << endl;
//     return unsat;
//   } else if (result == "unknown") {
//     Log(LOGSAT) << "Result is UNKNOWN" << endl;
//     Log(WARNING) << "Result is UNKNOWN" << endl;
//     return unknown;
//   } else if (result == "timeout") {
//     Log(LOGSAT) << "Result is timeout" << endl;
//     Log(WARNING) << "Result is timeout" << endl;
//     return unknown;
//   } else {
//     Log(ERROR) << "Internal error - Z3 did not return an expected satisfiability value." << endl << z3string;
//     Log(ERROR) << "Tried to smt the following constraints:" << endl;
//     for (int i = 0; i < (int)constraints.size(); ++i) {
//       Log(ERROR) << constraints[i]->toString() << endl;
//     }
//     fprintf(stderr, "Cannot interpret result returned by Z3: \"%s\".\n", result.c_str());
//     assert(0);
//     return unknown;
//   }
// }

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
