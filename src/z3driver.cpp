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

Z3_sort z3BoolSort;
Z3_sort z3IntSort;

using namespace std;

unsigned z3_symbol_count = 0;

Z3_ast z3_make_constant(Sort *sort)
{
  Z3_symbol symbol = Z3_mk_int_symbol(z3context, z3_symbol_count++);
  return Z3_mk_const(z3context, symbol, sort->interpretation);
}

Z3_ast z3_simplify(Term *term)
{
  return Z3_simplify(z3context, term->toSmt());
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

  // Z3_solver z3solver = Z3_mk_solver(z3context);
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
  Z3_solver z3solver = Z3_mk_solver(z3context);
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
  Z3_solver z3solver = Z3_mk_solver(z3context);
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

Term *unZ3(Z3_ast ast, Sort *sort)
{
  switch (Z3_get_ast_kind(z3context, ast)) {
  case Z3_APP_AST:
    {
    Z3_app app = Z3_to_app(z3context, ast);
    Z3_func_decl func_decl = Z3_get_app_decl(z3context, app);
    switch (Z3_get_decl_kind(z3context, func_decl)) {
    case Z3_OP_TRUE:
      return bTrue();
      break;
    case Z3_OP_FALSE:
      return bFalse();
      break;
    case Z3_OP_EQ:
      abortWithMessage("Cannot handle decl kind Z3_OP_EQ.");
      break;
    case Z3_OP_DISTINCT    :
      abortWithMessage("Cannot handle decl kind Z3_OP_DISTINCT    .");
      break;
    case Z3_OP_ITE :
      abortWithMessage("Cannot handle decl kind Z3_OP_ITE .");
      break;
    case Z3_OP_AND :
      abortWithMessage("Cannot handle decl kind Z3_OP_AND .");
      break;
    case Z3_OP_OR  :
      abortWithMessage("Cannot handle decl kind Z3_OP_OR  .");
      break;
    case Z3_OP_IFF :
      abortWithMessage("Cannot handle decl kind Z3_OP_IFF .");
      break;
    case Z3_OP_XOR :
      abortWithMessage("Cannot handle decl kind Z3_OP_XOR .");
      break;
    case Z3_OP_NOT :
      abortWithMessage("Cannot handle decl kind Z3_OP_NOT .");
      break;
    case Z3_OP_IMPLIES:
      abortWithMessage("Cannot handle decl kind Z3_OP_IMPLIES.");
      break;
    case Z3_OP_OEQ :
      abortWithMessage("Cannot handle decl kind Z3_OP_OEQ .");
      break;
    case Z3_OP_ANUM:
      abortWithMessage("Cannot handle decl kind Z3_OP_ANUM.");
      break;
    case Z3_OP_AGNUM:
      abortWithMessage("Cannot handle decl kind Z3_OP_AGNUM.");
      break;
    case Z3_OP_LE  :
      abortWithMessage("Cannot handle decl kind Z3_OP_LE  .");
      break;
    case Z3_OP_GE  :
      abortWithMessage("Cannot handle decl kind Z3_OP_GE  .");
      break;
    case Z3_OP_LT  :
      abortWithMessage("Cannot handle decl kind Z3_OP_LT  .");
      break;
    case Z3_OP_GT  :
      abortWithMessage("Cannot handle decl kind Z3_OP_GT  .");
      break;
    case Z3_OP_ADD :
      abortWithMessage("Cannot handle decl kind Z3_OP_ADD .");
      break;
    case Z3_OP_SUB :
      abortWithMessage("Cannot handle decl kind Z3_OP_SUB .");
      break;
    case Z3_OP_UMINUS:
      abortWithMessage("Cannot handle decl kind Z3_OP_UMINUS.");
      break;
    case Z3_OP_MUL :
      abortWithMessage("Cannot handle decl kind Z3_OP_MUL .");
      break;
    case Z3_OP_DIV :
      abortWithMessage("Cannot handle decl kind Z3_OP_DIV .");
      break;
    case Z3_OP_IDIV:
      abortWithMessage("Cannot handle decl kind Z3_OP_IDIV.");
      break;
    case Z3_OP_REM :
      abortWithMessage("Cannot handle decl kind Z3_OP_REM .");
      break;
    case Z3_OP_MOD :
      abortWithMessage("Cannot handle decl kind Z3_OP_MOD .");
      break;
    case Z3_OP_TO_REAL:
      abortWithMessage("Cannot handle decl kind Z3_OP_TO_REAL.");
      break;
    case Z3_OP_TO_INT:
      abortWithMessage("Cannot handle decl kind Z3_OP_TO_INT.");
      break;
    case Z3_OP_IS_INT:
      abortWithMessage("Cannot handle decl kind Z3_OP_IS_INT.");
      break;
    case Z3_OP_POWER:
      abortWithMessage("Cannot handle decl kind Z3_OP_POWER.");
      break;
    case Z3_OP_STORE:
      abortWithMessage("Cannot handle decl kind Z3_OP_STORE.");
      break;
    case Z3_OP_SELECT:
      abortWithMessage("Cannot handle decl kind Z3_OP_SELECT.");
      break;
    case Z3_OP_CONST_ARRAY :
      abortWithMessage("Cannot handle decl kind Z3_OP_CONST_ARRAY .");
      break;
    case Z3_OP_ARRAY_MAP   :
      abortWithMessage("Cannot handle decl kind Z3_OP_ARRAY_MAP   .");
      break;
    case Z3_OP_ARRAY_DEFAULT:
      abortWithMessage("Cannot handle decl kind Z3_OP_ARRAY_DEFAULT.");
      break;
    case Z3_OP_SET_UNION   :
      abortWithMessage("Cannot handle decl kind Z3_OP_SET_UNION   .");
      break;
    case Z3_OP_SET_INTERSECT:
      abortWithMessage("Cannot handle decl kind Z3_OP_SET_INTERSECT.");
      break;
    case Z3_OP_SET_DIFFERENCE:
      abortWithMessage("Cannot handle decl kind Z3_OP_SET_DIFFERENCE.");
      break;
    case Z3_OP_SET_COMPLEMENT:
      abortWithMessage("Cannot handle decl kind Z3_OP_SET_COMPLEMENT.");
      break;
    case Z3_OP_SET_SUBSET  :
      abortWithMessage("Cannot handle decl kind Z3_OP_SET_SUBSET  .");
      break;
    case Z3_OP_AS_ARRAY    :
      abortWithMessage("Cannot handle decl kind Z3_OP_AS_ARRAY    .");
      break;
    case Z3_OP_ARRAY_EXT   :
      abortWithMessage("Cannot handle decl kind Z3_OP_ARRAY_EXT   .");
      break;
    case Z3_OP_BNUM:
      abortWithMessage("Cannot handle decl kind Z3_OP_BNUM.");
      break;
    case Z3_OP_BIT1:
      abortWithMessage("Cannot handle decl kind Z3_OP_BIT1.");
      break;
    case Z3_OP_BIT0:
      abortWithMessage("Cannot handle decl kind Z3_OP_BIT0.");
      break;
    case Z3_OP_BNEG:
      abortWithMessage("Cannot handle decl kind Z3_OP_BNEG.");
      break;
    case Z3_OP_BADD:
      abortWithMessage("Cannot handle decl kind Z3_OP_BADD.");
      break;
    case Z3_OP_BSUB:
      abortWithMessage("Cannot handle decl kind Z3_OP_BSUB.");
      break;
    case Z3_OP_BMUL:
      abortWithMessage("Cannot handle decl kind Z3_OP_BMUL.");
      break;
    case Z3_OP_BSDIV:
      abortWithMessage("Cannot handle decl kind Z3_OP_BSDIV.");
      break;
    case Z3_OP_BUDIV:
      abortWithMessage("Cannot handle decl kind Z3_OP_BUDIV.");
      break;
    case Z3_OP_BSREM:
      abortWithMessage("Cannot handle decl kind Z3_OP_BSREM.");
      break;
    case Z3_OP_BUREM:
      abortWithMessage("Cannot handle decl kind Z3_OP_BUREM.");
      break;
    case Z3_OP_BSMOD:
      abortWithMessage("Cannot handle decl kind Z3_OP_BSMOD.");
      break;
    case Z3_OP_BSDIV0:
      abortWithMessage("Cannot handle decl kind Z3_OP_BSDIV0.");
      break;
    case Z3_OP_BUDIV0:
      abortWithMessage("Cannot handle decl kind Z3_OP_BUDIV0.");
      break;
    case Z3_OP_BSREM0:
      abortWithMessage("Cannot handle decl kind Z3_OP_BSREM0.");
      break;
    case Z3_OP_BUREM0:
      abortWithMessage("Cannot handle decl kind Z3_OP_BUREM0.");
      break;
    case Z3_OP_BSMOD0:
      abortWithMessage("Cannot handle decl kind Z3_OP_BSMOD0.");
      break;
    case Z3_OP_ULEQ:
      abortWithMessage("Cannot handle decl kind Z3_OP_ULEQ.");
      break;
    case Z3_OP_SLEQ:
      abortWithMessage("Cannot handle decl kind Z3_OP_SLEQ.");
      break;
    case Z3_OP_UGEQ:
      abortWithMessage("Cannot handle decl kind Z3_OP_UGEQ.");
      break;
    case Z3_OP_SGEQ:
      abortWithMessage("Cannot handle decl kind Z3_OP_SGEQ.");
      break;
    case Z3_OP_ULT :
      abortWithMessage("Cannot handle decl kind Z3_OP_ULT .");
      break;
    case Z3_OP_SLT :
      abortWithMessage("Cannot handle decl kind Z3_OP_SLT .");
      break;
    case Z3_OP_UGT :
      abortWithMessage("Cannot handle decl kind Z3_OP_UGT .");
      break;
    case Z3_OP_SGT :
      abortWithMessage("Cannot handle decl kind Z3_OP_SGT .");
      break;
    case Z3_OP_BAND:
      abortWithMessage("Cannot handle decl kind Z3_OP_BAND.");
      break;
    case Z3_OP_BOR :
      abortWithMessage("Cannot handle decl kind Z3_OP_BOR .");
      break;
    case Z3_OP_BNOT:
      abortWithMessage("Cannot handle decl kind Z3_OP_BNOT.");
      break;
    case Z3_OP_BXOR:
      abortWithMessage("Cannot handle decl kind Z3_OP_BXOR.");
      break;
    case Z3_OP_BNAND:
      abortWithMessage("Cannot handle decl kind Z3_OP_BNAND.");
      break;
    case Z3_OP_BNOR:
      abortWithMessage("Cannot handle decl kind Z3_OP_BNOR.");
      break;
    case Z3_OP_BXNOR:
      abortWithMessage("Cannot handle decl kind Z3_OP_BXNOR.");
      break;
    case Z3_OP_CONCAT:
      abortWithMessage("Cannot handle decl kind Z3_OP_CONCAT.");
      break;
    case Z3_OP_SIGN_EXT    :
      abortWithMessage("Cannot handle decl kind Z3_OP_SIGN_EXT    .");
      break;
    case Z3_OP_ZERO_EXT    :
      abortWithMessage("Cannot handle decl kind Z3_OP_ZERO_EXT    .");
      break;
    case Z3_OP_EXTRACT:
      abortWithMessage("Cannot handle decl kind Z3_OP_EXTRACT.");
      break;
    case Z3_OP_REPEAT:
      abortWithMessage("Cannot handle decl kind Z3_OP_REPEAT.");
      break;
    case Z3_OP_BREDOR:
      abortWithMessage("Cannot handle decl kind Z3_OP_BREDOR.");
      break;
    case Z3_OP_BREDAND:
      abortWithMessage("Cannot handle decl kind Z3_OP_BREDAND.");
      break;
    case Z3_OP_BCOMP:
      abortWithMessage("Cannot handle decl kind Z3_OP_BCOMP.");
      break;
    case Z3_OP_BSHL:
      abortWithMessage("Cannot handle decl kind Z3_OP_BSHL.");
      break;
    case Z3_OP_BLSHR:
      abortWithMessage("Cannot handle decl kind Z3_OP_BLSHR.");
      break;
    case Z3_OP_BASHR:
      abortWithMessage("Cannot handle decl kind Z3_OP_BASHR.");
      break;
    case Z3_OP_ROTATE_LEFT :
      abortWithMessage("Cannot handle decl kind Z3_OP_ROTATE_LEFT .");
      break;
    case Z3_OP_ROTATE_RIGHT:
      abortWithMessage("Cannot handle decl kind Z3_OP_ROTATE_RIGHT.");
      break;
    case Z3_OP_EXT_ROTATE_LEFT:
      abortWithMessage("Cannot handle decl kind Z3_OP_EXT_ROTATE_LEFT.");
      break;
    case Z3_OP_EXT_ROTATE_RIGHT    :
      abortWithMessage("Cannot handle decl kind Z3_OP_EXT_ROTATE_RIGHT    .");
      break;
    case Z3_OP_BIT2BOOL    :
      abortWithMessage("Cannot handle decl kind Z3_OP_BIT2BOOL    .");
      break;
    case Z3_OP_INT2BV:
      abortWithMessage("Cannot handle decl kind Z3_OP_INT2BV.");
      break;
    case Z3_OP_BV2INT:
      abortWithMessage("Cannot handle decl kind Z3_OP_BV2I.");
      break;
    case Z3_OP_CARRY:
      abortWithMessage("Cannot handle decl kind Z3_OP_CARRY.");
      break;
    case Z3_OP_XOR3:
      abortWithMessage("Cannot handle decl kind Z3_OP_XOR3.");
      break;
    case Z3_OP_BSMUL_NO_OVFL:
      abortWithMessage("Cannot handle decl kind Z3_OP_BSMUL_NO_OVFL.");
      break;
    case Z3_OP_BUMUL_NO_OVFL:
      abortWithMessage("Cannot handle decl kind Z3_OP_BUMUL_NO_OVFL.");
      break;
    case Z3_OP_BSMUL_NO_UDFL:
      abortWithMessage("Cannot handle decl kind Z3_OP_BSMUL_NO_UDFL.");
      break;
    case Z3_OP_BSDIV_I:
      abortWithMessage("Cannot handle decl kind Z3_OP_BSDIV_I.");
      break;
    case Z3_OP_BUDIV_I:
      abortWithMessage("Cannot handle decl kind Z3_OP_BUDIV_I.");
      break;
    case Z3_OP_BSREM_I:
      abortWithMessage("Cannot handle decl kind Z3_OP_BSREM_I.");
      break;
    case Z3_OP_BUREM_I:
      abortWithMessage("Cannot handle decl kind Z3_OP_BUREM_I.");
      break;
    case Z3_OP_BSMOD_I:
      abortWithMessage("Cannot handle decl kind Z3_OP_BSMOD_I.");
      break;
    case Z3_OP_PR_UNDEF    :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_UNDEF    .");
      break;
    case Z3_OP_PR_TRUE:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_TRUE.");
      break;
    case Z3_OP_PR_ASSERTED :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_ASSERTED .");
      break;
    case Z3_OP_PR_GOAL:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_GOAL.");
      break;
    case Z3_OP_PR_MODUS_PONENS:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_MODUS_PONENS.");
      break;
    case Z3_OP_PR_REFLEXIVITY:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_REFLEXIVITY.");
      break;
    case Z3_OP_PR_SYMMETRY :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_SYMMETRY .");
      break;
    case Z3_OP_PR_TRANSITIVITY:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_TRANSITIVITY.");
      break;
    case Z3_OP_PR_TRANSITIVITY_STAR:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_TRANSITIVITY_STAR.");
      break;
    case Z3_OP_PR_MONOTONICITY:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_MONOTONICITY.");
      break;
    case Z3_OP_PR_QUANT_INTRO:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_QUANT_INTRO.");
      break;
    case Z3_OP_PR_BIND:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_BIND.");
      break;
    case Z3_OP_PR_DISTRIBUTIVITY   :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_DISTRIBUTIVITY   .");
      break;
    case Z3_OP_PR_AND_ELIM :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_AND_ELIM .");
      break;
    case Z3_OP_PR_NOT_OR_ELIM:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_NOT_OR_ELIM.");
      break;
    case Z3_OP_PR_REWRITE  :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_REWRITE  .");
      break;
    case Z3_OP_PR_REWRITE_STAR:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_REWRITE_STAR.");
      break;
    case Z3_OP_PR_PULL_QUANT:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_PULL_QUANT.");
      break;
    case Z3_OP_PR_PUSH_QUANT:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_PUSH_QUANT.");
      break;
    case Z3_OP_PR_ELIM_UNUSED_VARS :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_ELIM_UNUSED_VARS .");
      break;
    case Z3_OP_PR_DER:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_DER.");
      break;
    case Z3_OP_PR_QUANT_INST:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_QUANT_INST.");
      break;
    case Z3_OP_PR_HYPOTHESIS:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_HYPOTHESIS.");
      break;
    case Z3_OP_PR_LEMMA    :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_LEMMA    .");
      break;
    case Z3_OP_PR_UNIT_RESOLUTION  :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_UNIT_RESOLUTION  .");
      break;
    case Z3_OP_PR_IFF_TRUE :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_IFF_TRUE .");
      break;
    case Z3_OP_PR_IFF_FALSE:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_IFF_FALSE.");
      break;
    case Z3_OP_PR_COMMUTATIVITY    :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_COMMUTATIVITY    .");
      break;
    case Z3_OP_PR_DEF_AXIOM:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_DEF_AXIOM.");
      break;
    case Z3_OP_PR_DEF_INTRO:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_DEF_INTRO.");
      break;
    case Z3_OP_PR_APPLY_DEF:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_APPLY_DEF.");
      break;
    case Z3_OP_PR_IFF_OEQ  :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_IFF_OEQ  .");
      break;
    case Z3_OP_PR_NNF_POS  :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_NNF_POS  .");
      break;
    case Z3_OP_PR_NNF_NEG  :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_NNF_NEG  .");
      break;
    case Z3_OP_PR_SKOLEMIZE:
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_SKOLEMIZE.");
      break;
    case Z3_OP_PR_MODUS_PONENS_OEQ :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_MODUS_PONENS_OEQ .");
      break;
    case Z3_OP_PR_TH_LEMMA :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_TH_LEMMA .");
      break;
    case Z3_OP_PR_HYPER_RESOLVE    :
      abortWithMessage("Cannot handle decl kind Z3_OP_PR_HYPER_RESOLVE    .");
      break;
    case Z3_OP_RA_STORE    :
      abortWithMessage("Cannot handle decl kind Z3_OP_RA_STORE    .");
      break;
    case Z3_OP_RA_EMPTY    :
      abortWithMessage("Cannot handle decl kind Z3_OP_RA_EMPTY    .");
      break;
    case Z3_OP_RA_IS_EMPTY :
      abortWithMessage("Cannot handle decl kind Z3_OP_RA_IS_EMPTY .");
      break;
    case Z3_OP_RA_JOIN:
      abortWithMessage("Cannot handle decl kind Z3_OP_RA_JOIN.");
      break;
    case Z3_OP_RA_UNION    :
      abortWithMessage("Cannot handle decl kind Z3_OP_RA_UNION    .");
      break;
    case Z3_OP_RA_WIDEN    :
      abortWithMessage("Cannot handle decl kind Z3_OP_RA_WIDEN    .");
      break;
    case Z3_OP_RA_PROJECT  :
      abortWithMessage("Cannot handle decl kind Z3_OP_RA_PROJECT  .");
      break;
    case Z3_OP_RA_FILTER   :
      abortWithMessage("Cannot handle decl kind Z3_OP_RA_FILTER   .");
      break;
    case Z3_OP_RA_NEGATION_FILTER  :
      abortWithMessage("Cannot handle decl kind Z3_OP_RA_NEGATION_FILTER  .");
      break;
    case Z3_OP_RA_RENAME   :
      abortWithMessage("Cannot handle decl kind Z3_OP_RA_RENAME   .");
      break;
    case Z3_OP_RA_COMPLEMENT:
      abortWithMessage("Cannot handle decl kind Z3_OP_RA_COMPLEMENT.");
      break;
    case Z3_OP_RA_SELECT   :
      abortWithMessage("Cannot handle decl kind Z3_OP_RA_SELECT   .");
      break;
    case Z3_OP_RA_CLONE    :
      abortWithMessage("Cannot handle decl kind Z3_OP_RA_CLONE    .");
      break;
    case Z3_OP_FD_CONSTANT :
      abortWithMessage("Cannot handle decl kind Z3_OP_FD_CONSTANT .");
      break;
    case Z3_OP_FD_LT:
      abortWithMessage("Cannot handle decl kind Z3_OP_FD_LT.");
      break;
    case Z3_OP_SEQ_UNIT    :
      abortWithMessage("Cannot handle decl kind Z3_OP_SEQ_UNIT    .");
      break;
    case Z3_OP_SEQ_EMPTY   :
      abortWithMessage("Cannot handle decl kind Z3_OP_SEQ_EMPTY   .");
      break;
    case Z3_OP_SEQ_CONCAT  :
      abortWithMessage("Cannot handle decl kind Z3_OP_SEQ_CONCAT  .");
      break;
    case Z3_OP_SEQ_PREFIX  :
      abortWithMessage("Cannot handle decl kind Z3_OP_SEQ_PREFIX  .");
      break;
    case Z3_OP_SEQ_SUFFIX  :
      abortWithMessage("Cannot handle decl kind Z3_OP_SEQ_SUFFIX  .");
      break;
    case Z3_OP_SEQ_CONTAINS:
      abortWithMessage("Cannot handle decl kind Z3_OP_SEQ_CONTAINS.");
      break;
    case Z3_OP_SEQ_EXTRACT :
      abortWithMessage("Cannot handle decl kind Z3_OP_SEQ_EXTRACT .");
      break;
    case Z3_OP_SEQ_REPLACE :
      abortWithMessage("Cannot handle decl kind Z3_OP_SEQ_REPLACE .");
      break;
    case Z3_OP_SEQ_AT:
      abortWithMessage("Cannot handle decl kind Z3_OP_SEQ_AT.");
      break;
    case Z3_OP_SEQ_LENGTH  :
      abortWithMessage("Cannot handle decl kind Z3_OP_SEQ_LENGTH  .");
      break;
    case Z3_OP_SEQ_INDEX   :
      abortWithMessage("Cannot handle decl kind Z3_OP_SEQ_INDEX   .");
      break;
    case Z3_OP_SEQ_TO_RE   :
      abortWithMessage("Cannot handle decl kind Z3_OP_SEQ_TO_RE   .");
      break;
    case Z3_OP_SEQ_IN_RE   :
      abortWithMessage("Cannot handle decl kind Z3_OP_SEQ_IN_RE   .");
      break;
    case Z3_OP_STR_TO_INT  :
      abortWithMessage("Cannot handle decl kind Z3_OP_STR_TO_INT  .");
      break;
    case Z3_OP_INT_TO_STR  :
      abortWithMessage("Cannot handle decl kind Z3_OP_INT_TO_STR  .");
      break;
    case Z3_OP_RE_PLUS:
      abortWithMessage("Cannot handle decl kind Z3_OP_RE_PLUS.");
      break;
    case Z3_OP_RE_STAR:
      abortWithMessage("Cannot handle decl kind Z3_OP_RE_STAR.");
      break;
    case Z3_OP_RE_OPTION   :
      abortWithMessage("Cannot handle decl kind Z3_OP_RE_OPTION   .");
      break;
    case Z3_OP_RE_CONCAT   :
      abortWithMessage("Cannot handle decl kind Z3_OP_RE_CONCAT   .");
      break;
    case Z3_OP_RE_UNION    :
      abortWithMessage("Cannot handle decl kind Z3_OP_RE_UNION    .");
      break;
    case Z3_OP_RE_RANGE    :
      abortWithMessage("Cannot handle decl kind Z3_OP_RE_RANGE    .");
      break;
    case Z3_OP_RE_LOOP:
      abortWithMessage("Cannot handle decl kind Z3_OP_RE_LOOP.");
      break;
    case Z3_OP_RE_INTERSECT:
      abortWithMessage("Cannot handle decl kind Z3_OP_RE_INTERSECT.");
      break;
    case Z3_OP_RE_EMPTY_SET:
      abortWithMessage("Cannot handle decl kind Z3_OP_RE_EMPTY_SET.");
      break;
    case Z3_OP_RE_FULL_SET :
      abortWithMessage("Cannot handle decl kind Z3_OP_RE_FULL_SET .");
      break;
    case Z3_OP_RE_COMPLEMENT:
      abortWithMessage("Cannot handle decl kind Z3_OP_RE_COMPLEMENT.");
      break;
    case Z3_OP_LABEL:
      abortWithMessage("Cannot handle decl kind Z3_OP_LABEL.");
      break;
    case Z3_OP_LABEL_LIT   :
      abortWithMessage("Cannot handle decl kind Z3_OP_LABEL_LIT   .");
      break;
    case Z3_OP_DT_CONSTRUCTOR:
      abortWithMessage("Cannot handle decl kind Z3_OP_DT_CONSTRUCTOR.");
      break;
    case Z3_OP_DT_RECOGNISER:
      abortWithMessage("Cannot handle decl kind Z3_OP_DT_RECOGNISER.");
      break;
    case Z3_OP_DT_IS:
      abortWithMessage("Cannot handle decl kind Z3_OP_DT_IS");
      break;
    case Z3_OP_DT_ACCESSOR :
      abortWithMessage("Cannot handle decl kind Z3_OP_DT_ACCESSOR .");
      break;
    case Z3_OP_DT_UPDATE_FIELD:
      abortWithMessage("Cannot handle decl kind Z3_OP_DT_UPDATE_FIELD.");
      break;
    case Z3_OP_PB_AT_MOST  :
      abortWithMessage("Cannot handle decl kind Z3_OP_PB_AT_MOST  .");
      break;
    case Z3_OP_PB_AT_LEAST :
      abortWithMessage("Cannot handle decl kind Z3_OP_PB_AT_LEAST .");
      break;
    case Z3_OP_PB_LE:
      abortWithMessage("Cannot handle decl kind Z3_OP_PB_LE.");
      break;
    case Z3_OP_PB_GE:
      abortWithMessage("Cannot handle decl kind Z3_OP_PB_GE.");
      break;
    case Z3_OP_PB_EQ:
      abortWithMessage("Cannot handle decl kind Z3_OP_PB_EQ.");
      break;
    case Z3_OP_FPA_RM_NEAREST_TIES_TO_EVEN :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_RM_NEAREST_TIES_TO_EVEN .");
      break;
    case Z3_OP_FPA_RM_NEAREST_TIES_TO_AWAY :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_RM_NEAREST_TIES_TO_AWAY .");
      break;
    case Z3_OP_FPA_RM_TOWARD_POSITIVE:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_RM_TOWARD_POSITIVE.");
      break;
    case Z3_OP_FPA_RM_TOWARD_NEGATIVE:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_RM_TOWARD_NEGATIVE.");
      break;
    case Z3_OP_FPA_RM_TOWARD_ZERO  :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_RM_TOWARD_ZERO  .");
      break;
    case Z3_OP_FPA_NUM:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_NUM.");
      break;
    case Z3_OP_FPA_PLUS_INF:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_PLUS_INF.");
      break;
    case Z3_OP_FPA_MINUS_INF:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_MINUS_INF.");
      break;
    case Z3_OP_FPA_NAN:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_NAN.");
      break;
    case Z3_OP_FPA_PLUS_ZERO:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_PLUS_ZERO.");
      break;
    case Z3_OP_FPA_MINUS_ZERO:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_MINUS_ZERO.");
      break;
    case Z3_OP_FPA_ADD:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_ADD.");
      break;
    case Z3_OP_FPA_SUB:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_SUB.");
      break;
    case Z3_OP_FPA_NEG:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_NEG.");
      break;
    case Z3_OP_FPA_MUL:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_MUL.");
      break;
    case Z3_OP_FPA_DIV:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_DIV.");
      break;
    case Z3_OP_FPA_REM:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_REM.");
      break;
    case Z3_OP_FPA_ABS:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_ABS.");
      break;
    case Z3_OP_FPA_MIN:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_MIN.");
      break;
    case Z3_OP_FPA_MAX:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_MAX.");
      break;
    case Z3_OP_FPA_FMA:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_FMA.");
      break;
    case Z3_OP_FPA_SQRT    :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_SQRT    .");
      break;
    case Z3_OP_FPA_ROUND_TO_INTEGRAL:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_ROUND_TO_INTEGRAL.");
      break;
    case Z3_OP_FPA_EQ:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_EQ.");
      break;
    case Z3_OP_FPA_LT:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_LT.");
      break;
    case Z3_OP_FPA_GT:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_GT.");
      break;
    case Z3_OP_FPA_LE:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_LE.");
      break;
    case Z3_OP_FPA_GE:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_GE.");
      break;
    case Z3_OP_FPA_IS_NAN  :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_IS_NAN  .");
      break;
    case Z3_OP_FPA_IS_INF  :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_IS_INF  .");
      break;
    case Z3_OP_FPA_IS_ZERO :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_IS_ZERO .");
      break;
    case Z3_OP_FPA_IS_NORMAL:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_IS_NORMAL.");
      break;
    case Z3_OP_FPA_IS_SUBNORMAL    :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_IS_SUBNORMAL    .");
      break;
    case Z3_OP_FPA_IS_NEGATIVE:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_IS_NEGATIVE.");
      break;
    case Z3_OP_FPA_IS_POSITIVE:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_IS_POSITIVE.");
      break;
    case Z3_OP_FPA_FP:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_FP.");
      break;
    case Z3_OP_FPA_TO_FP   :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_TO_FP   .");
      break;
    case Z3_OP_FPA_TO_FP_UNSIGNED  :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_TO_FP_UNSIGNED  .");
      break;
    case Z3_OP_FPA_TO_UBV  :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_TO_UBV  .");
      break;
    case Z3_OP_FPA_TO_SBV  :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_TO_SBV  .");
      break;
    case Z3_OP_FPA_TO_REAL :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_TO_REAL .");
      break;
    case Z3_OP_FPA_TO_IEEE_BV:
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_TO_IEEE_BV.");
      break;
    case Z3_OP_FPA_BVWRAP  :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_BVWRAP  .");
      break;
    case Z3_OP_FPA_BV2RM   :
      abortWithMessage("Cannot handle decl kind Z3_OP_FPA_BV2RM   .");
      break;
    case Z3_OP_INTERNAL    :
      abortWithMessage("Cannot handle decl kind Z3_OP_INTERNAL    .");
      break;
    case Z3_OP_UNINTERPRETED:
      abortWithMessage("Cannot handle decl kind Z3_OP_UNINTERPRETED.");
      break;
    }
    }
    break;
  case Z3_NUMERAL_AST:
    abortWithMessage("Cannot unz3 an ast of kind Z3_NUMERAL_AST.");
    break;
  case Z3_VAR_AST:
    abortWithMessage("Cannot unz3 an ast of kind Z3_VAR_AST.");
    break;
  case Z3_QUANTIFIER_AST:
    abortWithMessage("Cannot unz3 an ast of kind Z3_QUANTIFIER_AST.");
    break;
  case Z3_SORT_AST:
    abortWithMessage("Cannot unz3 an ast of kind Z3_SORT_AST.");
    break;
  case Z3_FUNC_DECL_AST:
    abortWithMessage("Cannot unz3 an ast of kind Z3_FUNC_DECL_AST.");
    break;
  case Z3_UNKNOWN_AST:
    abortWithMessage("Cannot unz3 an ast of kind Z3_UNKNOWN_AST.");
    break;
  }
  assert(0);
  return 0;
}
