#ifndef FASTTERM_H__
#define FASTTERM_H__

#include <cstdlib>
#include <cassert>
#include <vector>
#include <z3.h>

#define MAXVARS (1024 * 16)
#define MAXFUNCS 2048
#define MAXDATA (1024 * 1024 * 32)
#define MAXARITYDATA (1024 * 16)
#define MAXSORTS 1024
#define MAXSUBSORTINGS 1024
#define MAXSUBST (1024 * 1024 * 8)

enum BuiltinSortType {
  bltnBool,
  bltnInt
};

enum BuiltinFuncType {
  bltnAnd,
  bltnOr,
  bltnNot,
  bltnTrue,
  bltnFalse
};

typedef unsigned int uint32;

typedef unsigned int uint;

typedef uint32 FastVar;   /* 0 .. MAXVARS - 1 */
typedef uint32 FastFunc;  /* 0 .. MAXFUNCS - 1 */
typedef uint32 FastTerm;  /* 0 .. MAXVARS - 1 and MAXVARS .. */
                          /* if >= MAXVARS, then it represents a pointer into termData */
typedef uint32 FastSort;  /* 0 .. MAXSORTS - 1 */

struct FastSubst {
  uint32 size;
  uint32 count;
  uint32 *data;

  FastSubst();
  ~FastSubst();
  void addToSubst(FastVar var, FastTerm term);
  bool inDomain(FastVar var);
  FastTerm range(FastVar var);
  FastTerm applySubst(FastTerm term);
  void composeWith(FastVar v, FastTerm t);
};

struct FastSubst1 {
  std::vector<uint32> data;

  FastSubst1();
  ~FastSubst1();
  void addToSubst1(FastVar var, FastTerm term);
  bool inDomain1(FastVar var);
  FastTerm range1(FastVar var);
  FastTerm applySubst1(FastTerm term);
  void composeWith1(FastVar v, FastTerm t);
};

struct FastSubst2 : public std::vector<uint32> {
  FastSubst2();
  ~FastSubst2();
  void addToSubst2(FastVar var, FastTerm term);
  bool inDomain2(FastVar var);
  FastTerm range2(FastVar var);
  FastTerm applySubst2(FastTerm term);
  void composeWith2(FastVar v, FastTerm t);
};

/*
  Initialization.
 */
void initFastTerm();
void initSorts();
void initFuncs();

/*
  Sorts.
 */
FastSort newSort(const char *name);

bool validFastSort(FastSort sort);

bool isBuiltinSort(FastSort sort);

BuiltinSortType getBuiltinSortType(FastSort sort);

FastSort fastBoolSort();
FastSort fastIntSort();

void newSubSort(FastSort subsort, FastSort supersort);

const char *getSortName(FastSort sort);

/*
  Variables.
 */
FastVar newVar(const char *name, FastSort sort);
bool validFastVar(FastVar var);
const char *getVarName(FastVar var);
FastSort getVarSort(FastVar var);
bool eq_var(FastVar var1, FastVar var2);

/*
  Function symbols.
*/
FastFunc newConst(const char *name, FastSort sort);
FastFunc newFunc(const char *name, FastSort resultSort, uint32 arity, FastSort *args);
bool validFastFunc(FastFunc func);

FastSort getArgSort(FastFunc func, uint arg);
const char *getFuncName(FastFunc);
uint getArity(FastFunc func);
FastSort getFuncSort(FastFunc func);

bool isBuiltinFunc(FastFunc func);
BuiltinFuncType getBuiltinFuncType(FastFunc func);

bool eq_func(FastFunc func1, FastFunc func2);

/*
  Terms.
*/
FastTerm newFuncTerm(FastFunc func, FastTerm *args);
bool validFastTerm(FastTerm term);

bool isVariable(FastTerm term);
bool isFuncTerm(FastTerm term);

size_t printTerm(FastTerm term, char *buffer, size_t size);

FastFunc getFunc(FastTerm term);
FastTerm *args(FastTerm term);
FastTerm getArg(FastTerm term, uint arg);
FastSort getSort(FastTerm term);

bool eq_term(FastTerm term1, FastTerm term2);

/*
  Builtins.
 */
FastTerm fastAnd(FastTerm, FastTerm);
FastTerm fastOr(FastTerm, FastTerm);
FastTerm fastNot(FastTerm);
FastTerm fastTrue();
FastTerm fastFalse();

/*
  Substitutions.
*/

FastTerm applyUnitySubst(FastTerm term, FastVar v, FastTerm t);
size_t printSubst(FastSubst &subst, char *buffer, size_t size);
size_t printSubst1(FastSubst1 &subst, char *buffer, size_t size);
size_t printSubst2(FastSubst1 &subst, char *buffer, size_t size);

/*
  Helper functions.
*/

bool occurs(FastVar var, FastTerm term);

/*
  Syntactic unification.
 */
bool unify(FastTerm t1, FastTerm t2, FastSubst &result);
bool unify1(FastTerm t1, FastTerm t2, FastSubst1 &result);
bool unify2(FastTerm t1, FastTerm t2, FastSubst2 &result);

bool match(FastTerm subject, FastTerm pattern, FastSubst &result);

/*
  Z3 interface.
 */
Z3_context init_z3_context();
Z3_ast toZ3Term(Z3_context, FastTerm);
Z3_sort toZ3Sort(Z3_context, FastSort);
Z3_lbool z3_sat_check(Z3_context context, FastTerm term);
Z3_lbool z3_check(Z3_context context, Z3_solver solver);
Z3_solver init_z3_solver(Z3_context context);
void done_z3_solver(Z3_context context, Z3_solver solver);
void z3_assert(Z3_context context, Z3_solver solver, FastTerm term);
void z3_push(Z3_context context, Z3_solver solver);
void z3_pop(Z3_context context, Z3_solver solver);

#endif
