#include "fastterm.h"
#include "log.h"
#include <cassert>
#include <map>
#include <utility>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

std::vector<FastTerm> z3_asserts;

void fastterm_z3_error_handler(Z3_context context, Z3_error_code error)
{
  Z3_string string_error = Z3_get_error_msg(context, error);
  printf("Error in calling Z3 API: %s\n", string_error);
  exit(0);
}

extern int builtinFuncExtra[MAXFUNCS];
extern FastFunc funcFalse;
extern uint32 funcCount;

std::map<std::pair<Z3_context, FastFunc>, Z3_func_decl> z3func;
std::map<std::pair<Z3_context, FastTerm>, Z3_ast> cacheTerm;
std::map<std::pair<Z3_context, FastTerm>, Z3_func_decl> cacheFunc;
std::map<std::pair<Z3_context, FastTerm>, Z3_sort> cacheSort;
map<pair<Z3_context, Z3_symbol>, Z3_func_decl> symbol_to_func_decl;
map<pair<Z3_context, Z3_func_decl>, FastFunc> func_decl_to_function;

Z3_context init_z3_context()
{
  Z3_config z3config = Z3_mk_config();
  Z3_set_param_value(z3config, "timeout", "200");
  Z3_set_param_value(z3config, "auto_config", "true");
  
  Z3_context z3context = Z3_mk_context(z3config);
  Z3_set_error_handler(z3context, fastterm_z3_error_handler);

  for (uint32 func = 0; func < funcCount; ++func) {
    if (isBuiltinFunc(func) && getBuiltinFuncType(func) == bltnUD) {
      //      cerr << "Creating Z3 function symbol " << getFuncName(func) << endl;
      assert(getArity(func) < 16);
      Z3_sort domain[16];
      uint size = getArity(func);
      for (uint i = 0; i < size; ++i) {
	//	cerr << "Argument #" << (i + 1) << ": " << getSortName(getArgSort(func, i)) << endl;
	assert(isBuiltinSort(getArgSort(func, i)));
	domain[i] = toZ3Sort(z3context, getArgSort(func, i));
      }
      assert(isBuiltinSort(getFuncSort(func)));
      Z3_sort range = toZ3Sort(z3context, getFuncSort(func));
      //      cerr << "Result sort: " << getSortName(getFuncSort(func)) << endl;
      Z3_symbol symbol = Z3_mk_string_symbol(z3context, getFuncName(func));
      Z3_func_decl result = Z3_mk_func_decl(z3context, symbol, size, domain, range);
      symbol_to_func_decl[make_pair(z3context, symbol)] = result;
      func_decl_to_function[make_pair(z3context, result)] = func;
      z3func[make_pair(z3context, func)] = result;
    }
  }

  return z3context;
}

map<pair<Z3_context, Z3_sort>, FastSort> sort_from_z3sort;

Z3_sort toZ3Sort(Z3_context context, FastSort sort)
{
  if (cacheSort.find(std::make_pair(context, sort)) != cacheSort.end()) {
    return cacheSort[std::make_pair(context, sort)];
  } else {
    Z3_sort result;
    if (isBuiltinSort(sort)) {
      switch (getBuiltinSortType(sort)) {
      case bltnBool:
	result = Z3_mk_bool_sort(context);
	break;
      case bltnInt:
	result = Z3_mk_int_sort(context);
	break;
      case bltnArray:
	{
	  extern FastSort sortArguments[MAXSORTS][4]; // currently arguments for array
	  Z3_sort domainSort = toZ3Sort(context, sortArguments[sort][0]);
	  Z3_sort rangeSort = toZ3Sort(context, sortArguments[sort][1]);
	  result = Z3_mk_array_sort(context, domainSort, rangeSort);
	}
	break;
      default:
	assert(0);
	result = Z3_mk_bool_sort(context);
	break;
      }
    } else {
      Z3_symbol symbol = Z3_mk_string_symbol(context, getSortName(sort));
      result = Z3_mk_uninterpreted_sort(context, symbol);
    }
    cacheSort[std::make_pair(context, sort)] = result;
    sort_from_z3sort[make_pair(context, result)] = sort;
    LOG(DEBUG9, cerr << "map z3 sort " << Z3_ast_to_string(context, Z3_sort_to_ast(context, result)) << " to "<< getSortName(sort));
    return result;
  }
}

Z3_func_decl toZ3FuncDecl(Z3_context context, FastFunc func)
{
  exit(-1);
  if (cacheFunc.find(std::make_pair(context, func)) != cacheFunc.end()) {
    return cacheFunc[std::make_pair(context, func)];
  } else {
    Z3_func_decl result;
    Z3_sort returnSort = toZ3Sort(context, getFunc(func));
    Z3_sort args[16];
    assert(getArity(func) < 16);
    if (getArity(func) >= 16) {
      exit(-1);
    }
    for (uint i = 0; i < getArity(func); ++i) {
      args[i] = toZ3Sort(context, getArgSort(func, i));
    }
    Z3_symbol symbol = Z3_mk_string_symbol(context, getFuncName(func));
    result = Z3_mk_func_decl(context, symbol, getArity(func), args, returnSort);
    cacheFunc[std::make_pair(context, func)] = result;
    return result;
  }
}

map<pair<Z3_context, Z3_symbol>, FastVar> z3_const_to_var;
map<pair<Z3_context, Z3_symbol>, FastTerm> z3_const_to_const;

Z3_ast toZ3Term(Z3_context context, FastTerm term)
{
  toZ3Sort(context, getSort(term));
  // char buffer[1024];
  // printTerm(term, buffer, 1024);
  // printf("toZ3Term %s\n", buffer);
  if (cacheTerm.find(std::make_pair(context, term)) != cacheTerm.end()) {
    Z3_ast result = cacheTerm[std::make_pair(context, term)];
    //    printf("toZ3Term returned %s (cached)\n", Z3_ast_to_string(context, result));
    return result;
  } else {
    Z3_ast result;
    if (isVariable(term)) {
      Z3_symbol symbol = Z3_mk_string_symbol(context, getVarName(term));
      z3_const_to_var[make_pair(context, symbol)] = term;
      result = Z3_mk_const(context, symbol, toZ3Sort(context, getVarSort(term)));
    } else {
      assert(isFuncTerm(term));
      FastFunc func = getFunc(term);
      Z3_ast args[16];
      assert(getArity(func) < 16);
      if (getArity(func) >= 16) {
	exit(-1);
      }
      for (uint i = 0; i < getArity(func); ++i) {
	args[i] = toZ3Term(context, getArg(term, i));
      }
      if (isBuiltinFunc(func)) {
	switch (getBuiltinFuncType(func)) {
	case bltnAnd:
	  assert(getArity(func) == 2);
	  result = Z3_mk_and(context, 2, args);
	  break;
	case bltnImplies:
	  assert(getArity(func) == 2);
	  result = Z3_mk_implies(context, args[0], args[1]);
	  break;
	case bltnOr:
	  assert(getArity(func) == 2);
	  result = Z3_mk_or(context, 2, args);
	  break;
	case bltnNot:
	  assert(getArity(func) == 1);
	  result = Z3_mk_not(context, args[0]);
	  break;
	case bltnTrue:
	  assert(getArity(func) == 0);
	  result = Z3_mk_true(context);
	  break;
	case bltnFalse:
	  assert(getArity(func) == 0);
	  result = Z3_mk_false(context);
	  break;
	case bltnNumeral:
	  assert(getArity(func) == 0);
	  result = Z3_mk_numeral(context, std::to_string(builtinFuncExtra[func]).c_str(), toZ3Sort(context, fastIntSort()));
	  break;
	case bltnLE:
	  assert(getArity(func) == 2);
	  result = Z3_mk_le(context, args[0], args[1]);
	  break;
	case bltnPlus:
	  assert(getArity(func) == 2);
	  result = Z3_mk_add(context, 2, args);
	  break;
	case bltnTimes:
	  assert(getArity(func) == 2);
	  result = Z3_mk_mul(context, 2, args);
	  break;
	case bltnDiv:
	  assert(getArity(func) == 2);
	  result = Z3_mk_div(context, args[0], args[1]);
	  break;
	case bltnMod:
	  assert(getArity(func) == 2);
	  result = Z3_mk_mod(context, args[0], args[1]);
	  break;
	case bltnMinus:
	  assert(getArity(func) == 2);
	  result = Z3_mk_sub(context, 2, args);
	  break;
	case bltnEqInt:
	case bltnEqBool:
	  assert(getArity(func) == 2);
	  result = Z3_mk_eq(context, args[0], args[1]);
	  break;
	case bltnUD:
	  //	  cerr << "making bltnUD app" << endl;
	  assert(z3func.find(std::make_pair(context, func)) != z3func.end());
	  result = Z3_mk_app(context, z3func[make_pair(context, func)], getArity(func), args);
	  //	  cerr << "done making bltnUD app" << endl;
	  break;
	case bltnIte:
	  result = Z3_mk_ite(context, args[0], args[1], args[2]);
	  break;
	case bltnForall:
	  {
	    Z3_app bound[4];
	    assert(Z3_get_ast_kind(context, args[0]) == Z3_APP_AST);
	    Z3_pattern patterns[4];
	    bound[0] = (Z3_app)args[0];
	    result = Z3_mk_forall_const(context, 0, 1, bound, 0, patterns, args[1]);
	  }
	  break;
	case bltnExists:
	  {
	    Z3_app bound[4];
	    Z3_pattern patterns[4];
	    assert(Z3_get_ast_kind(context, args[0]) == Z3_APP_AST);
	    bound[0] = (Z3_app)args[0];
	    result = Z3_mk_exists_const(context, 0, 1, bound, 0, patterns, args[1]);
	  }
	  break;
	case bltnFreshConstant:
	  {
	    Z3_symbol symbol = Z3_mk_string_symbol(context, getFuncName(func));
	    result = Z3_mk_const(context, symbol, toZ3Sort(context, getSort(term)));
	  }
	  break;
	case bltnSelect:
	  result = Z3_mk_select(context, args[0], args[1]);
	  break;
	case bltnStore:
	  result = Z3_mk_store(context, args[0], args[1], args[2]);
	  break;
	case bltnConstArray:
	  {
	    FastSort arraySort = getFuncSort(func);
	    extern FastSort sortArguments[MAXSORTS][4];
	    assert(isBuiltinSort(arraySort));
	    assert(getBuiltinSortType(arraySort) == bltnArray);
	    result = Z3_mk_const_array(context, toZ3Sort(context, sortArguments[arraySort][0]),
				       args[0]);
	  }
	  break;
	case bltnEqArray:
	  result = Z3_mk_eq(context, args[0], args[1]);
	  break;
	default:
	  std::cerr << "Error: don't know how to translate " <<
	    getFuncName(func) << " (bltn = " << getBuiltinFuncType(func) << ")" << endl;
	  assert(0);
	  result = Z3_mk_false(context);
	  break;
	}
      } else {
	//	printf("5 toZ3Term %d\n", term);
	result = Z3_mk_app(context, toZ3FuncDecl(context, func), getArity(func), args);
      }
    }
    cacheTerm[std::make_pair(context, term)] = result;
    //    printf("toZ3Term returned %s\n", Z3_ast_to_string(context, result));
    return result;
  }
}

Z3_lbool z3_sat_check(Z3_context context, FastTerm term)
{
  Z3_solver solver = init_z3_solver(context);
  for (uint i = 0; i < z3_asserts.size(); ++i) {
    z3_assert(context, solver, z3_asserts[i]);
  }
  z3_assert(context, solver, term);
  //  cerr << "Starting Z3 query" << endl;
  Z3_lbool result = z3_check(context, solver);
  //  cerr << "Done Z3 query" << endl;
  done_z3_solver(context, solver);
  return result;
}

Z3_solver init_z3_solver(Z3_context context)
{
  Z3_solver solver = Z3_mk_solver(context);
  Z3_solver_inc_ref(context, solver);
  return solver;
}

void done_z3_solver(Z3_context context, Z3_solver solver)
{
  Z3_solver_dec_ref(context, solver);
}

void z3_assert(Z3_context context, Z3_solver solver, FastTerm term)
{
  assert(getSort(term) == fastBoolSort());
  Z3_ast z3_term = toZ3Term(context, term);
  Z3_solver_assert(context, solver, z3_term);
  //  printf("asserting %s\n", Z3_ast_to_string(context, z3_term));
}

Z3_lbool z3_check(Z3_context context, Z3_solver solver)
{
  Z3_lbool result = Z3_solver_check(context, solver);
  return result;
}

void z3_push(Z3_context context, Z3_solver solver)
{
  Z3_solver_push(context, solver);
}

void z3_pop(Z3_context context, Z3_solver solver)
{
  Z3_solver_pop(context, solver, 1);
}

/*
Z3_ast z3_simplify(FastTerm term, Z3_context z3context)
{
  Z3_params simplifyParams = Z3_mk_params(z3context);
  Z3_params_set_bool(z3context, simplifyParams, Z3_mk_string_symbol(z3context, "sort_store"), true);
  Z3_params_inc_ref(z3context, simplifyParams);

  Z3_tactic simplifyTactic = Z3_mk_tactic(z3context, "ctx-solver-simplify");
  Z3_tactic_inc_ref(z3context, simplifyTactic);

}
*/

void add_z3_assert(FastTerm term)
{
  z3_asserts.push_back(term);
}

//int count = 0;

FastTerm simplify(FastTerm term)
{
  if (isVariable(term)) {
    return term;
  } else {
    FastSort sort = getSort(term);
    if (isBuiltinSort(sort)) {
      return simplifyBuiltin(term);
    } else {
      FastFunc func = getFunc(term);
      vector<FastTerm> newargs;
      bool changed = false;
      for (uint i = 0; i < getArity(func); ++i) {
	newargs.push_back(simplify(getArg(term, i)));
	if (!eq_term(newargs[i], getArg(term, i))) {
	  changed = true;
	}
      }
      if (changed) {
	return newFuncTerm(func, &newargs[0]);
      } else {
	return term;
      }
    }
  }
}

FastTerm simplifyBuiltin(FastTerm term)
{
  assert(isBuiltinSort(getSort(term)));
  LOG(DEBUG9, cerr << "Simplifying " << toString(term));
  LOG(DEBUG9, cerr << "of sort " << getSortName(getSort(term)));
  //  return simplifyFast(term);

  //  printf("simplify %d\n", count++);
  //  term = simplifyFast(term);
  //  return term;

  Z3_context z3context = init_z3_context();

  Z3_params simplifyParams = Z3_mk_params(z3context);
  Z3_params_inc_ref(z3context, simplifyParams);
  Z3_params_set_bool(z3context, simplifyParams, Z3_mk_string_symbol(z3context, "sort_store"), true);

  Z3_params tacticParams = Z3_mk_params(z3context);
  Z3_params_inc_ref(z3context, tacticParams);
  // Z3_params_set_uint(z3context, tacticParams, Z3_mk_string_symbol(z3context, "timeout"), 2000);

  Z3_tactic simplifyTactic = Z3_mk_tactic(z3context, "ctx-solver-simplify");
  Z3_tactic_inc_ref(z3context, simplifyTactic);
  
  Z3_ast toSimplify = toZ3Term(z3context, term);
  if (getSort(term) == fastBoolSort()) {
    Z3_goal goal = Z3_mk_goal(z3context, false, false, false);
    Z3_goal_inc_ref(z3context, goal);
    Z3_goal_assert(z3context, goal, toSimplify);
    Z3_apply_result res = Z3_tactic_apply_ex(z3context, simplifyTactic, goal, tacticParams);
    Z3_apply_result_inc_ref(z3context, res);
    std::vector<Z3_ast> clauses;
    int nrgoals = Z3_apply_result_get_num_subgoals(z3context, res);
    for (int i = 0; i < nrgoals; ++i) {
      Z3_goal g = Z3_apply_result_get_subgoal(z3context, res, i);
      Z3_goal_inc_ref(z3context, g);
      int sz = Z3_goal_size(z3context, g);
      for (int j = 0; j < sz; ++j) {
        clauses.push_back(Z3_goal_formula(z3context, g, j));
      }
      Z3_goal_dec_ref(z3context, g);
    }
    Z3_ast result = clauses.empty() ? Z3_mk_true(z3context) : (
      (clauses.size() == 1) ? clauses[0] :
      Z3_mk_and(z3context, clauses.size(), &clauses[0])
      );
    toSimplify = result;
    Z3_goal_dec_ref(z3context, goal);
    Z3_apply_result_dec_ref(z3context, res);
  }
  Z3_ast z3result = Z3_simplify_ex(z3context, toSimplify, simplifyParams);
  std::vector<FastVar> boundVars;
  return unZ3(z3result, getSort(term), boundVars, z3context);
}

#include <map>

using namespace std;

FastTerm unZ3(Z3_ast ast, FastSort sort, vector<FastVar> boundVars, Z3_context z3context)
{
  LOG(DEBUG8, cerr << "UnZ3-ing " << Z3_ast_to_string(z3context, ast) << ".");
  LOG(DEBUG8, cerr << "of sort " << getSortName(sort) << ".");
  switch (Z3_get_ast_kind(z3context, ast)) {
  case Z3_APP_AST:
    {
      LOG(DEBUG8, cerr << "UnZ3: entering Z3_APP_AST.");
      Z3_app app = Z3_to_app(z3context, ast);
      Z3_func_decl func_decl = Z3_get_app_decl(z3context, app);
      switch (Z3_get_decl_kind(z3context, func_decl)) {
      case Z3_OP_UNINTERPRETED:
	{
	  LOG(DEBUG8, cerr << "UnZ3: entering Z3_OP_UNINTERPRETED.");
	  Z3_symbol symbol = Z3_get_decl_name(z3context, func_decl);
	  if (z3_const_to_var.find(make_pair(z3context, symbol)) != z3_const_to_var.end()) {
	    FastTerm resultUnZ3 = z3_const_to_var[make_pair(z3context, symbol)];
	    LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	    return resultUnZ3;
	  } else if (z3_const_to_const.find(make_pair(z3context, symbol)) != z3_const_to_const.end()) {
	    FastTerm resultUnZ3 = z3_const_to_const[make_pair(z3context, symbol)];
	    LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	    return resultUnZ3;
	  } else {
	    assert(symbol_to_func_decl.find(make_pair(z3context, symbol)) != symbol_to_func_decl.end());
	    Z3_func_decl func = symbol_to_func_decl[make_pair(z3context, symbol)];
	    assert(func_decl_to_function.find(make_pair(z3context, func)) != func_decl_to_function.end());
	    FastFunc function = func_decl_to_function[make_pair(z3context, func)];
	    unsigned size = Z3_get_app_num_args(z3context, app);
	    vector<FastTerm> arguments;
	    for (uint i = 0; i < size; ++i) {
	      arguments.push_back(unZ3(Z3_get_app_arg(z3context, app, i), getArgSort(function, i), boundVars, z3context));
	    }
	    FastTerm resultUnZ3 = newFuncTerm(function, &arguments[0]);
	    LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	    return resultUnZ3;
	  }
	}
	break;
      case Z3_OP_TRUE:
	{
	  FastTerm resultUnZ3 = fastTrue();
	  LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	  return resultUnZ3;
    }
	break;
      case Z3_OP_FALSE:
	{
	  FastTerm resultUnZ3 = fastFalse();
	  LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	  return resultUnZ3;
	}
	break;
      case Z3_OP_LE:
	{
	  assert(sort == fastBoolSort());
	  Z3_app app = Z3_to_app(z3context, ast);
	  if (Z3_get_app_num_args(z3context, app) != 2) {
	    abortWithMessage("Expected 2 arguments in Z3_OP_LE application.");
	  }
	  Z3_ast arg1 = Z3_get_app_arg(z3context, app, 0);
	  Z3_ast arg2 = Z3_get_app_arg(z3context, app, 1);
	  FastTerm resultUnZ3 = fastLE(unZ3(arg1, fastIntSort(), boundVars, z3context), unZ3(arg2, fastIntSort(), boundVars, z3context));
	  LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	  return resultUnZ3;
	}
	break;
      case Z3_OP_EQ:
	{
	  assert(sort == fastBoolSort());
	  Z3_app app = Z3_to_app(z3context, ast);
	  if (Z3_get_app_num_args(z3context, app) != 2) {
	    abortWithMessage("Expected 2 arguments in Z3_OP_EQ application.");
	  }
	  Z3_ast arg1 = Z3_get_app_arg(z3context, app, 0);
	  Z3_ast arg2 = Z3_get_app_arg(z3context, app, 1);
	  assert(sort_from_z3sort.find(make_pair(z3context, Z3_get_sort(z3context, arg1))) != sort_from_z3sort.end());
	  assert(sort_from_z3sort.find(make_pair(z3context, Z3_get_sort(z3context, arg2))) != sort_from_z3sort.end());
	  FastSort sa1 = sort_from_z3sort[make_pair(z3context, Z3_get_sort(z3context, arg1))];
	  FastSort sa2 = sort_from_z3sort[make_pair(z3context, Z3_get_sort(z3context, arg2))];
	  //	  FastSort sa1 = getSortByName(Z3_sort_to_string(z3context, ));
	  //	  FastSort sa2 = getSortByName(Z3_sort_to_string(z3context, Z3_get_sort(z3context, arg2)));
	  if (sa1 != sa2) {
	    abortWithMessage("Equality for different sorts in Z3_OP_EQ application.");
	  }
	  FastTerm resultUnZ3 = fastEq(unZ3(arg1, sa1, boundVars, z3context), unZ3(arg2, sa2, boundVars, z3context));
	  LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	  return resultUnZ3;
	}
	break;
      case Z3_OP_DISTINCT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_DISTINCT    .");
	break;
      case Z3_OP_ITE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_ITE .");
	break;
      case Z3_OP_AND:
	{
	  assert(sort == fastBoolSort());
	  Z3_app app = Z3_to_app(z3context, ast);
	  if (Z3_get_app_num_args(z3context, app) < 2) {
	    abortWithMessage("Expected >= 2 arguments in Z3_OP_AND application.");
	  }
	  vector<FastTerm> args;
	  for (uint i = 0; i < Z3_get_app_num_args(z3context, app); ++i) {
	    args.push_back(unZ3(Z3_get_app_arg(z3context, app, i), fastBoolSort(), boundVars, z3context));
	  }
	  FastTerm resultUnZ3 = fastAndVector(args);
	  LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	  return resultUnZ3;
	}
	break;
      case Z3_OP_OR:
	{
	  assert(sort == fastBoolSort());
	  Z3_app app = Z3_to_app(z3context, ast);
	  if (Z3_get_app_num_args(z3context, app) < 2) {
	    abortWithMessage("Expected 2 arguments in Z3_OP_OR application.");
	  }
	  
	  vector<FastTerm> args;
	  for (int i = 0; i < static_cast<int>(Z3_get_app_num_args(z3context, app)); ++i) {
	    args.push_back(unZ3(Z3_get_app_arg(z3context, app, i), fastBoolSort(), boundVars, z3context));
	  }
	  FastTerm resultUnZ3 = fastOrVector(args);
	  LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	  return resultUnZ3;
	}
	break;
      case Z3_OP_IFF:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_IFF .");
	break;
      case Z3_OP_XOR:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_XOR .");
	break;
      case Z3_OP_NOT:
	{
	  assert(sort == fastBoolSort());
	  Z3_app app = Z3_to_app(z3context, ast);
	  if (Z3_get_app_num_args(z3context, app) != 1) {
	    abortWithMessage("Expected 1 argument in Z3_OP_NOT application.");
	  }
	  Z3_ast arg1 = Z3_get_app_arg(z3context, app, 0);
	  FastTerm resultUnZ3 = fastNot(unZ3(arg1, fastBoolSort(), boundVars, z3context));
	  LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	  return resultUnZ3;
	}
	break;
      case Z3_OP_IMPLIES:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_IMPLIES.");
	break;
      case Z3_OP_OEQ:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_OEQ .");
	break;
      case Z3_OP_ANUM:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_ANUM.");
	break;
      case Z3_OP_AGNUM:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_AGNUM.");
	break;
      case Z3_OP_GE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_GE  .");
	break;
      case Z3_OP_LT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_LT  .");
	break;
      case Z3_OP_GT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_GT  .");
	break;
      case Z3_OP_ADD:
	{
	  assert(sort == fastIntSort());
	  Z3_app app = Z3_to_app(z3context, ast);
	  if (Z3_get_app_num_args(z3context, app) < 2) {
	    abortWithMessage("Expected >= 2 arguments in Z3_OP_ADD application.");
	  }
	  vector<FastTerm > args;
	  for (uint i = 0; i < Z3_get_app_num_args(z3context, app); ++i) {
	    args.push_back(unZ3(Z3_get_app_arg(z3context, app, i), fastIntSort(), boundVars, z3context));
	  }
	  FastTerm resultUnZ3 = fastPlusVector(args);
	  LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	  return resultUnZ3;
	}
	break;
      case Z3_OP_SUB:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SUB .");
	break;
      case Z3_OP_UMINUS:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_UMINUS.");
	break;
      case Z3_OP_MUL:
	{
	  assert(sort == fastIntSort());
	  Z3_app app = Z3_to_app(z3context, ast);
	  if (Z3_get_app_num_args(z3context, app) < 2) {
	    abortWithMessage("Expected >= 2 arguments in Z3_OP_MUL application.");
	  }
	  vector<FastTerm > args;
	  for (int i = 0; i < static_cast<int>(Z3_get_app_num_args(z3context, app)); ++i) {
	    args.push_back(unZ3(Z3_get_app_arg(z3context, app, i), fastIntSort(), boundVars, z3context));
	  }
	  FastTerm resultUnZ3 = fastTimesVector(args);
	  LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	  return resultUnZ3;
	}
	break;
      case Z3_OP_DIV:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_DIV .");
	break;
      case Z3_OP_IDIV:
	{
	  assert(sort == fastIntSort());
	  Z3_app app = Z3_to_app(z3context, ast);
	  if (Z3_get_app_num_args(z3context, app) != 2) {
	    abortWithMessage("Expected 2 arguments in Z3_OP_IDIV application.");
	  }
	  Z3_ast arg1 = Z3_get_app_arg(z3context, app, 0);
	  Z3_ast arg2 = Z3_get_app_arg(z3context, app, 1);
	  FastTerm resultUnZ3 = fastDiv(unZ3(arg1, fastIntSort(), boundVars, z3context), unZ3(arg2, fastIntSort(), boundVars, z3context));
	  LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	  return resultUnZ3;
	}
	break;
      case Z3_OP_REM:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_REM .");
	break;
      case Z3_OP_MOD:
	{
	  assert(sort == fastIntSort());
	  Z3_app app = Z3_to_app(z3context, ast);
	  if (Z3_get_app_num_args(z3context, app) != 2) {
	    abortWithMessage("Expected 2 arguments in Z3_OP_MOD application.");
	  }
	  Z3_ast arg1 = Z3_get_app_arg(z3context, app, 0);
	  Z3_ast arg2 = Z3_get_app_arg(z3context, app, 1);
	  FastTerm resultUnZ3 = fastMod(unZ3(arg1, fastIntSort(), boundVars, z3context), unZ3(arg2, fastIntSort(), boundVars, z3context));
	  LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	  return resultUnZ3;
	}
	break;
      case Z3_OP_TO_REAL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_TO_REAL.");
	break;
      case Z3_OP_TO_INT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_TO_INT.");
	break;
      case Z3_OP_IS_INT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_IS_INT.");
	break;
      case Z3_OP_POWER:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_POWER.");
	break;
      case Z3_OP_STORE:
	{
	  LOG(DEBUG8, cerr << "UnZ3: entering Z3_OP_STORE.");
	  extern map<pair<FastSort, pair<FastSort, FastSort>>, FastFunc> storeFunc;
	  Z3_app app = Z3_to_app(z3context, ast);
	  if (Z3_get_app_num_args(z3context, app) != 3) {
	    abortWithMessage("Expected 3 arguments in Z3_OP_STORE application.");
	  }
	  Z3_ast arg1 = Z3_get_app_arg(z3context, app, 0);
	  Z3_ast arg2 = Z3_get_app_arg(z3context, app, 1);
	  Z3_ast arg3 = Z3_get_app_arg(z3context, app, 2);
	  assert(sort_from_z3sort.find(make_pair(z3context, Z3_get_sort(z3context, arg1))) != sort_from_z3sort.end());
	  assert(sort_from_z3sort.find(make_pair(z3context, Z3_get_sort(z3context, arg2))) != sort_from_z3sort.end());
	  assert(sort_from_z3sort.find(make_pair(z3context, Z3_get_sort(z3context, arg3))) != sort_from_z3sort.end());
	  FastSort sa1 = sort_from_z3sort[make_pair(z3context, Z3_get_sort(z3context, arg1))];
	  FastSort sa2 = sort_from_z3sort[make_pair(z3context, Z3_get_sort(z3context, arg2))];
	  FastSort sa3 = sort_from_z3sort[make_pair(z3context, Z3_get_sort(z3context, arg3))];
	  LOG(DEBUG9, cerr << "store sort 1 = " << getSortName(sa1));
	  LOG(DEBUG9, cerr << "store sort 2 = " << getSortName(sa2));
	  LOG(DEBUG9, cerr << "store sort 3 = " << getSortName(sa3));

	  //	  FastSort sa1 = getSortByName(Z3_sort_to_string(z3context, Z3_get_sort(z3context, arg1)));
	  //	  FastSort sa2 = getSortByName(Z3_sort_to_string(z3context, Z3_get_sort(z3context, arg2)));
	  //	  FastSort sa3 = getSortByName(Z3_sort_to_string(z3context, Z3_get_sort(z3context, arg3)));
	  pair<FastSort, pair<FastSort, FastSort>> key = make_pair(sa1, make_pair(sa2, sa3));
	  FastFunc fun = storeFunc[key];
	  FastTerm newargs[3];
	  newargs[0] = unZ3(arg1, sa1, boundVars, z3context);
	  newargs[1] = unZ3(arg2, sa2, boundVars, z3context);
	  newargs[2] = unZ3(arg3, sa3, boundVars, z3context);
	  FastTerm resultUnZ3 = newFuncTerm(fun, newargs);
	  LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	  return resultUnZ3;
	}
	break;
      case Z3_OP_SELECT:
	{
	  extern map<pair<FastSort, pair<FastSort, FastSort>>, FastFunc> selectFunc;
	  Z3_app app = Z3_to_app(z3context, ast);
	  if (Z3_get_app_num_args(z3context, app) != 2) {
	    abortWithMessage("Expected 2 arguments in Z3_OP_SELECT application.");
	  }
	  Z3_ast arg1 = Z3_get_app_arg(z3context, app, 0);
	  Z3_ast arg2 = Z3_get_app_arg(z3context, app, 1);
	  assert(sort_from_z3sort.find(make_pair(z3context, Z3_get_sort(z3context, arg1))) != sort_from_z3sort.end());
	  assert(sort_from_z3sort.find(make_pair(z3context, Z3_get_sort(z3context, arg2))) != sort_from_z3sort.end());
	  FastSort sa1 = sort_from_z3sort[make_pair(z3context, Z3_get_sort(z3context, arg1))];
	  FastSort sa2 = sort_from_z3sort[make_pair(z3context, Z3_get_sort(z3context, arg2))];
	  // FastSort sa1 = getSortByName(Z3_sort_to_string(z3context, Z3_get_sort(z3context, arg1)));
	  // FastSort sa2 = getSortByName(Z3_sort_to_string(z3context, Z3_get_sort(z3context, arg2)));
	  pair<FastSort, pair<FastSort, FastSort>> key = make_pair(sort, make_pair(sa1, sa2));
	  FastFunc fun = selectFunc[key];
	  FastTerm newargs[3];
	  newargs[0] = unZ3(arg1, sa1, boundVars, z3context);
	  newargs[1] = unZ3(arg2, sa2, boundVars, z3context);
	  FastTerm resultUnZ3 = newFuncTerm(fun, newargs);
	  LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	  return resultUnZ3;
	}
	break;
      case Z3_OP_CONST_ARRAY:
	{
	extern std::map<std::pair<FastSort, FastSort>, FastFunc> constArrayFunc;
	  Z3_app app = Z3_to_app(z3context, ast);
	  if (Z3_get_app_num_args(z3context, app) != 1) {
	    abortWithMessage("Expected 1 arguments in Z3_OP_CONST_ARRAY application.");
	  }
	  Z3_ast arg1 = Z3_get_app_arg(z3context, app, 0);
	  FastSort sa1 = sort_from_z3sort[make_pair(z3context, Z3_get_sort(z3context, arg1))];
	  LOG(DEBUG8, cerr << "CONSTARRAY argument = " << getSortName(sa1) << ".");
	  LOG(DEBUG8, cerr << "CONSTARRAY result = " << getSortName(sort) << ".");
	  pair<FastSort, FastSort> key = make_pair(sort, sa1);
	  FastFunc fun = constArrayFunc[key];
	  FastTerm newargs[3];
	  newargs[0] = unZ3(arg1, sa1, boundVars, z3context);
	  FastTerm resultUnZ3 = newFuncTerm(fun, newargs);
	  LOG(DEBUG8, cerr << "Using func = " << getFuncName(fun));
	  LOG(DEBUG8, cerr << "CONSTARRAY Result of unZ3 = " << toString(resultUnZ3) << ".");
	  return resultUnZ3;
	}
	break;
      case Z3_OP_ARRAY_MAP:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_ARRAY_MAP   .");
	break;
      case Z3_OP_ARRAY_DEFAULT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_ARRAY_DEFAULT.");
	break;
      case Z3_OP_SET_UNION:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SET_UNION   .");
	break;
      case Z3_OP_SET_INTERSECT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SET_INTERSECT.");
	break;
      case Z3_OP_SET_DIFFERENCE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SET_DIFFERENCE.");
	break;
      case Z3_OP_SET_COMPLEMENT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SET_COMPLEMENT.");
	break;
      case Z3_OP_SET_SUBSET:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SET_SUBSET  .");
	break;
      case Z3_OP_AS_ARRAY:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_AS_ARRAY    .");
	break;
      case Z3_OP_ARRAY_EXT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_ARRAY_EXT   .");
	break;
      case Z3_OP_BNUM:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BNUM.");
	break;
      case Z3_OP_BIT1:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BIT1.");
	break;
      case Z3_OP_BIT0:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BIT0.");
	break;
      case Z3_OP_BNEG:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BNEG.");
	break;
      case Z3_OP_BADD:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BADD.");
	break;
      case Z3_OP_BSUB:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BSUB.");
	break;
      case Z3_OP_BMUL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BMUL.");
	break;
      case Z3_OP_BSDIV:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BSDIV.");
	break;
      case Z3_OP_BUDIV:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BUDIV.");
	break;
      case Z3_OP_BSREM:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BSREM.");
	break;
      case Z3_OP_BUREM:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BUREM.");
	break;
      case Z3_OP_BSMOD:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BSMOD.");
	break;
      case Z3_OP_BSDIV0:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BSDIV0.");
	break;
      case Z3_OP_BUDIV0:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BUDIV0.");
	break;
      case Z3_OP_BSREM0:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BSREM0.");
	break;
      case Z3_OP_BUREM0:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BUREM0.");
	break;
      case Z3_OP_BSMOD0:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BSMOD0.");
	break;
      case Z3_OP_ULEQ:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_ULEQ.");
	break;
      case Z3_OP_SLEQ:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SLEQ.");
	break;
      case Z3_OP_UGEQ:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_UGEQ.");
	break;
      case Z3_OP_SGEQ:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SGEQ.");
	break;
      case Z3_OP_ULT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_ULT .");
	break;
      case Z3_OP_SLT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SLT .");
	break;
      case Z3_OP_UGT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_UGT .");
	break;
      case Z3_OP_SGT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SGT .");
	break;
      case Z3_OP_BAND:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BAND.");
	break;
      case Z3_OP_BOR:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BOR .");
	break;
      case Z3_OP_BNOT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BNOT.");
	break;
      case Z3_OP_BXOR:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BXOR.");
	break;
      case Z3_OP_BNAND:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BNAND.");
	break;
      case Z3_OP_BNOR:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BNOR.");
	break;
      case Z3_OP_BXNOR:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BXNOR.");
	break;
      case Z3_OP_CONCAT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_CONCAT.");
	break;
      case Z3_OP_SIGN_EXT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SIGN_EXT    .");
	break;
      case Z3_OP_ZERO_EXT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_ZERO_EXT    .");
	break;
      case Z3_OP_EXTRACT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_EXTRACT.");
	break;
      case Z3_OP_REPEAT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_REPEAT.");
	break;
      case Z3_OP_BREDOR:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BREDOR.");
	break;
      case Z3_OP_BREDAND:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BREDAND.");
	break;
      case Z3_OP_BCOMP:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BCOMP.");
	break;
      case Z3_OP_BSHL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BSHL.");
	break;
      case Z3_OP_BLSHR:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BLSHR.");
	break;
      case Z3_OP_BASHR:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BASHR.");
	break;
      case Z3_OP_ROTATE_LEFT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_ROTATE_LEFT .");
	break;
      case Z3_OP_ROTATE_RIGHT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_ROTATE_RIGHT.");
	break;
      case Z3_OP_EXT_ROTATE_LEFT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_EXT_ROTATE_LEFT.");
	break;
      case Z3_OP_EXT_ROTATE_RIGHT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_EXT_ROTATE_RIGHT    .");
	break;
      case Z3_OP_BIT2BOOL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BIT2BOOL    .");
	break;
      case Z3_OP_INT2BV:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INT2BV.");
	break;
      case Z3_OP_BV2INT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BV2I.");
	break;
      case Z3_OP_CARRY:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_CARRY.");
	break;
      case Z3_OP_XOR3:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_XOR3.");
	break;
      case Z3_OP_BSMUL_NO_OVFL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BSMUL_NO_OVFL.");
	break;
      case Z3_OP_BUMUL_NO_OVFL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BUMUL_NO_OVFL.");
	break;
      case Z3_OP_BSMUL_NO_UDFL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BSMUL_NO_UDFL.");
	break;
      case Z3_OP_BSDIV_I:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BSDIV_I.");
	break;
      case Z3_OP_BUDIV_I:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BUDIV_I.");
	break;
      case Z3_OP_BSREM_I:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BSREM_I.");
	break;
      case Z3_OP_BUREM_I:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BUREM_I.");
	break;
      case Z3_OP_BSMOD_I:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_BSMOD_I.");
	break;
      case Z3_OP_PR_UNDEF:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_UNDEF    .");
	break;
      case Z3_OP_PR_TRUE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_TRUE.");
	break;
      case Z3_OP_PR_ASSERTED:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_ASSERTED .");
	break;
      case Z3_OP_PR_GOAL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_GOAL.");
	break;
      case Z3_OP_PR_MODUS_PONENS:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_MODUS_PONENS.");
	break;
      case Z3_OP_PR_REFLEXIVITY:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_REFLEXIVITY.");
	break;
      case Z3_OP_PR_SYMMETRY:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_SYMMETRY .");
	break;
      case Z3_OP_PR_TRANSITIVITY:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_TRANSITIVITY.");
	break;
      case Z3_OP_PR_TRANSITIVITY_STAR:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_TRANSITIVITY_STAR.");
	break;
      case Z3_OP_PR_MONOTONICITY:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_MONOTONICITY.");
	break;
      case Z3_OP_PR_QUANT_INTRO:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_QUANT_INTRO.");
	break;
      case Z3_OP_PR_BIND:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_BIND.");
	break;
      case Z3_OP_PR_DISTRIBUTIVITY:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_DISTRIBUTIVITY   .");
	break;
      case Z3_OP_PR_AND_ELIM:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_AND_ELIM .");
	break;
      case Z3_OP_PR_NOT_OR_ELIM:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_NOT_OR_ELIM.");
	break;
      case Z3_OP_PR_REWRITE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_REWRITE  .");
	break;
      case Z3_OP_PR_REWRITE_STAR:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_REWRITE_STAR.");
	break;
      case Z3_OP_PR_PULL_QUANT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_PULL_QUANT.");
	break;
      case Z3_OP_PR_PUSH_QUANT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_PUSH_QUANT.");
	break;
      case Z3_OP_PR_ELIM_UNUSED_VARS:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_ELIM_UNUSED_VARS .");
	break;
      case Z3_OP_PR_DER:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_DER.");
	break;
      case Z3_OP_PR_QUANT_INST:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_QUANT_INST.");
	break;
      case Z3_OP_PR_HYPOTHESIS:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_HYPOTHESIS.");
	break;
      case Z3_OP_PR_LEMMA:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_LEMMA    .");
	break;
      case Z3_OP_PR_UNIT_RESOLUTION:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_UNIT_RESOLUTION  .");
	break;
      case Z3_OP_PR_IFF_TRUE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_IFF_TRUE .");
	break;
      case Z3_OP_PR_IFF_FALSE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_IFF_FALSE.");
	break;
      case Z3_OP_PR_COMMUTATIVITY:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_COMMUTATIVITY    .");
	break;
      case Z3_OP_PR_DEF_AXIOM:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_DEF_AXIOM.");
	break;
      case Z3_OP_PR_DEF_INTRO:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_DEF_INTRO.");
	break;
      case Z3_OP_PR_APPLY_DEF:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_APPLY_DEF.");
	break;
      case Z3_OP_PR_IFF_OEQ:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_IFF_OEQ  .");
	break;
      case Z3_OP_PR_NNF_POS:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_NNF_POS  .");
	break;
      case Z3_OP_PR_NNF_NEG:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_NNF_NEG  .");
	break;
      case Z3_OP_PR_SKOLEMIZE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_SKOLEMIZE.");
	break;
      case Z3_OP_PR_MODUS_PONENS_OEQ:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_MODUS_PONENS_OEQ .");
	break;
      case Z3_OP_PR_TH_LEMMA:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_TH_LEMMA .");
	break;
      case Z3_OP_PR_HYPER_RESOLVE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PR_HYPER_RESOLVE    .");
	break;
      case Z3_OP_RA_STORE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RA_STORE    .");
	break;
      case Z3_OP_RA_EMPTY:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RA_EMPTY    .");
	break;
      case Z3_OP_RA_IS_EMPTY:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RA_IS_EMPTY .");
	break;
      case Z3_OP_RA_JOIN:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RA_JOIN.");
	break;
      case Z3_OP_RA_UNION:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RA_UNION    .");
	break;
      case Z3_OP_RA_WIDEN:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RA_WIDEN    .");
	break;
      case Z3_OP_RA_PROJECT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RA_PROJECT  .");
	break;
      case Z3_OP_RA_FILTER:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RA_FILTER   .");
	break;
      case Z3_OP_RA_NEGATION_FILTER:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RA_NEGATION_FILTER  .");
	break;
      case Z3_OP_RA_RENAME:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RA_RENAME   .");
	break;
      case Z3_OP_RA_COMPLEMENT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RA_COMPLEMENT.");
	break;
      case Z3_OP_RA_SELECT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RA_SELECT   .");
	break;
      case Z3_OP_RA_CLONE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RA_CLONE    .");
	break;
      case Z3_OP_FD_CONSTANT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FD_CONSTANT .");
	break;
      case Z3_OP_FD_LT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FD_LT.");
	break;
      case Z3_OP_SEQ_UNIT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SEQ_UNIT    .");
	break;
      case Z3_OP_SEQ_EMPTY:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SEQ_EMPTY   .");
	break;
      case Z3_OP_SEQ_CONCAT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SEQ_CONCAT  .");
	break;
      case Z3_OP_SEQ_PREFIX:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SEQ_PREFIX  .");
	break;
      case Z3_OP_SEQ_SUFFIX:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SEQ_SUFFIX  .");
	break;
      case Z3_OP_SEQ_CONTAINS:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SEQ_CONTAINS.");
	break;
      case Z3_OP_SEQ_EXTRACT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SEQ_EXTRACT .");
	break;
      case Z3_OP_SEQ_REPLACE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SEQ_REPLACE .");
	break;
      case Z3_OP_SEQ_AT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SEQ_AT.");
	break;
      case Z3_OP_SEQ_LENGTH:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SEQ_LENGTH  .");
	break;
      case Z3_OP_SEQ_INDEX:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SEQ_INDEX   .");
	break;
      case Z3_OP_SEQ_TO_RE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SEQ_TO_RE   .");
	break;
      case Z3_OP_SEQ_IN_RE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_SEQ_IN_RE   .");
	break;
      case Z3_OP_STR_TO_INT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_STR_TO_INT  .");
	break;
      case Z3_OP_INT_TO_STR:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INT_TO_STR  .");
	break;
      case Z3_OP_RE_PLUS:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RE_PLUS.");
	break;
      case Z3_OP_RE_STAR:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RE_STAR.");
	break;
      case Z3_OP_RE_OPTION:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RE_OPTION   .");
	break;
      case Z3_OP_RE_CONCAT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RE_CONCAT   .");
	break;
      case Z3_OP_RE_UNION:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RE_UNION    .");
	break;
      case Z3_OP_RE_RANGE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RE_RANGE    .");
	break;
      case Z3_OP_RE_LOOP:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RE_LOOP.");
	break;
      case Z3_OP_RE_INTERSECT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RE_INTERSECT.");
	break;
      case Z3_OP_RE_EMPTY_SET:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RE_EMPTY_SET.");
	break;
      case Z3_OP_RE_FULL_SET:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RE_FULL_SET .");
	break;
      case Z3_OP_RE_COMPLEMENT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_RE_COMPLEMENT.");
	break;
      case Z3_OP_LABEL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_LABEL.");
	break;
      case Z3_OP_LABEL_LIT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_LABEL_LIT   .");
	break;
      case Z3_OP_DT_CONSTRUCTOR:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_DT_CONSTRUCTOR.");
	break;
      case Z3_OP_DT_RECOGNISER:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_DT_RECOGNISER.");
	break;
      case Z3_OP_DT_IS:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_DT_IS");
	break;
      case Z3_OP_DT_ACCESSOR:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_DT_ACCESSOR .");
	break;
      case Z3_OP_DT_UPDATE_FIELD:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_DT_UPDATE_FIELD.");
	break;
      case Z3_OP_PB_AT_MOST:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PB_AT_MOST  .");
	break;
      case Z3_OP_PB_AT_LEAST:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PB_AT_LEAST .");
	break;
      case Z3_OP_PB_LE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PB_LE.");
	break;
      case Z3_OP_PB_GE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PB_GE.");
	break;
      case Z3_OP_PB_EQ:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_PB_EQ.");
	break;
      case Z3_OP_FPA_RM_NEAREST_TIES_TO_EVEN:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_RM_NEAREST_TIES_TO_EVEN .");
	break;
      case Z3_OP_FPA_RM_NEAREST_TIES_TO_AWAY:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_RM_NEAREST_TIES_TO_AWAY .");
	break;
      case Z3_OP_FPA_RM_TOWARD_POSITIVE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_RM_TOWARD_POSITIVE.");
	break;
      case Z3_OP_FPA_RM_TOWARD_NEGATIVE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_RM_TOWARD_NEGATIVE.");
	break;
      case Z3_OP_FPA_RM_TOWARD_ZERO:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_RM_TOWARD_ZERO  .");
	break;
      case Z3_OP_FPA_NUM:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_NUM.");
	break;
      case Z3_OP_FPA_PLUS_INF:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_PLUS_INF.");
	break;
      case Z3_OP_FPA_MINUS_INF:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_MINUS_INF.");
	break;
      case Z3_OP_FPA_NAN:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_NAN.");
	break;
      case Z3_OP_FPA_PLUS_ZERO:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_PLUS_ZERO.");
	break;
      case Z3_OP_FPA_MINUS_ZERO:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_MINUS_ZERO.");
	break;
      case Z3_OP_FPA_ADD:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_ADD.");
	break;
      case Z3_OP_FPA_SUB:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_SUB.");
	break;
      case Z3_OP_FPA_NEG:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_NEG.");
	break;
      case Z3_OP_FPA_MUL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_MUL.");
	break;
      case Z3_OP_FPA_DIV:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_DIV.");
	break;
      case Z3_OP_FPA_REM:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_REM.");
	break;
      case Z3_OP_FPA_ABS:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_ABS.");
	break;
      case Z3_OP_FPA_MIN:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_MIN.");
	break;
      case Z3_OP_FPA_MAX:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_MAX.");
	break;
      case Z3_OP_FPA_FMA:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_FMA.");
	break;
      case Z3_OP_FPA_SQRT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_SQRT    .");
	break;
      case Z3_OP_FPA_ROUND_TO_INTEGRAL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_ROUND_TO_INTEGRAL.");
	break;
      case Z3_OP_FPA_EQ:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_EQ.");
	break;
      case Z3_OP_FPA_LT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_LT.");
	break;
      case Z3_OP_FPA_GT:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_GT.");
	break;
      case Z3_OP_FPA_LE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_LE.");
	break;
      case Z3_OP_FPA_GE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_GE.");
	break;
      case Z3_OP_FPA_IS_NAN:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_IS_NAN  .");
	break;
      case Z3_OP_FPA_IS_INF:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_IS_INF  .");
	break;
      case Z3_OP_FPA_IS_ZERO:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_IS_ZERO .");
	break;
      case Z3_OP_FPA_IS_NORMAL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_IS_NORMAL.");
	break;
      case Z3_OP_FPA_IS_SUBNORMAL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_IS_SUBNORMAL    .");
	break;
      case Z3_OP_FPA_IS_NEGATIVE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_IS_NEGATIVE.");
	break;
      case Z3_OP_FPA_IS_POSITIVE:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_IS_POSITIVE.");
	break;
      case Z3_OP_FPA_FP:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_FP.");
	break;
      case Z3_OP_FPA_TO_FP:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_TO_FP   .");
	break;
      case Z3_OP_FPA_TO_FP_UNSIGNED:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_TO_FP_UNSIGNED  .");
	break;
      case Z3_OP_FPA_TO_UBV:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_TO_UBV  .");
	break;
      case Z3_OP_FPA_TO_SBV:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_TO_SBV  .");
	break;
      case Z3_OP_FPA_TO_REAL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_TO_REAL .");
	break;
      case Z3_OP_FPA_TO_IEEE_BV:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_TO_IEEE_BV.");
	break;
      case Z3_OP_FPA_BVWRAP:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_BVWRAP  .");
	break;
      case Z3_OP_FPA_BV2RM:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_FPA_BV2RM   .");
	break;
      case Z3_OP_INTERNAL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INTERNAL    .");
	break;
  /*
      case Z3_OP_PR_ASSUMPTION_ADD:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INTERNAL    .");
	break;
      case Z3_OP_PR_LEMMA_ADD:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INTERNAL    .");
	break;
      case Z3_OP_PR_REDUNDANT_DEL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INTERNAL    .");
	break;
      case Z3_OP_PR_CLAUSE_TRAIL:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INTERNAL    .");
	break;
      case Z3_OP_SEQ_NTH:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INTERNAL    .");
	break;
      case Z3_OP_SEQ_LAST_INDEX:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INTERNAL    .");
	break;
      case Z3_OP_SPECIAL_RELATION_LO:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INTERNAL    .");
	break;
      case Z3_OP_SPECIAL_RELATION_PO:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INTERNAL    .");
	break;
      case Z3_OP_SPECIAL_RELATION_PLO:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INTERNAL    .");
	break;
      case Z3_OP_SPECIAL_RELATION_TO:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INTERNAL    .");
	break;
      case Z3_OP_SPECIAL_RELATION_TC:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INTERNAL    .");
	break;
      case Z3_OP_SPECIAL_RELATION_TRC:
	abortWithMessage("In unZ3, cannot handle decl kind Z3_OP_INTERNAL    .");
	break;
  */
      default:
        LOG(DEBUG8, cerr << "unZ3: undefined case.");
  break;
      }
      break;
    }
  case Z3_NUMERAL_AST:
    {
      int result;
      if (Z3_get_numeral_int(z3context, ast, &result)) {
	string funname = std::to_string(result);
	
	if (!existsFunc(funname.c_str())) {
	  /* TODO: handle this case as well
	     if (!function) {
	     vector<FastSort > arguments;
	     extern map<string, FastFunc > functions;
	     functions[funname] = new Function(funname, arguments, getIntSort(), new z3_ct(result));
	     function = getFunction(funname);
	     assert(function != 0);
	     }
	  */
	  cerr << "Cannot find function " << funname << endl;
	  assert(0);
	  exit(-1);
	}
	FastFunc function = getFuncByName(funname.c_str());
	assert(getArity(function) == 0);
	FastTerm resultUnZ3 = newFuncTerm(function, nullptr);
	LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
	return resultUnZ3;
      } else {
	abortWithMessage("For ast of kind Z3_NUMERAL_AST, cannot retrieve numeral outside of int range.");
      }
    }
    break;
  case Z3_VAR_AST:
    {
      unsigned index = Z3_get_index_value(z3context, ast);
      LOG(DEBUG8, cerr << "Got index of " << index << " and boundVars.size() = " << boundVars.size() << ".");
      if (index >= boundVars.size()) {
	abortWithMessage("Error in unz3-ing an ast of kind Z3_VAR_AST (bound variable out of range).");
      }
      FastTerm resultUnZ3 = boundVars[boundVars.size() - 1 - index];
      LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
      return resultUnZ3;
    }
    break;
  case Z3_QUANTIFIER_AST:
    {
      LOG(DEBUG8, cerr << "UnZ3-ing quantifier");
      int nrBoundVars = (int)Z3_get_quantifier_num_bound(z3context, ast);
      vector<FastVar> newBoundVars = boundVars;
      vector<FastVar> freshBoundVars;
      for (int bvarnum = 0; bvarnum < nrBoundVars; ++bvarnum) {
	Z3_symbol symbol = Z3_get_quantifier_bound_name(z3context, ast, bvarnum);
	assert(z3_const_to_var.find(make_pair(z3context, symbol)) != z3_const_to_var.end());
	FastVar boundVar = z3_const_to_var[make_pair(z3context, symbol)];
	newBoundVars.push_back(boundVar);
	freshBoundVars.push_back(boundVar);
      }
      
      Z3_ast body = Z3_get_quantifier_body(z3context, ast);
      FastTerm bodyTerm = unZ3(body, fastBoolSort(), newBoundVars, z3context);
      FastTerm (*whichQ)(FastVar, FastTerm) = NULL;
      if (Z3_is_quantifier_forall(z3context, ast)) {
	whichQ = fastForall;
      } else if (Z3_is_quantifier_exists(z3context, ast)) {
	whichQ = fastExists;
      } else {
	abortWithMessage("In UnZ3: unknown quantifier.");
      }
      reverse(freshBoundVars.begin(), freshBoundVars.end());
      FastTerm resultUnZ3 = bodyTerm;
      for (const auto &boundVar : freshBoundVars)
	resultUnZ3 = whichQ(boundVar, resultUnZ3);
      LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
      return resultUnZ3;
    }
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
  default:
    LOG(DEBUG8, cerr << "unZ3: undefined case.");
    break;
  }
  assert(0);
  FastTerm resultUnZ3 = 0;
  LOG(DEBUG8, cerr << "Result of unZ3 = " << toString(resultUnZ3) << ".");
  return resultUnZ3;
}
