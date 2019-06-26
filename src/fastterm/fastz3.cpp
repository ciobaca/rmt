#include "fastterm.h"
#include <cassert>
#include <map>
#include <utility>
#include <string>
#include <vector>
#include <iostream>

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
      z3func[make_pair(z3context, func)] = result;
    }
  }

  return z3context;
}

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

Z3_ast toZ3Term(Z3_context context, FastTerm term)
{
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
	default:
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
