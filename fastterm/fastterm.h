#ifndef FASTTERM_H__
#define FASTTERM_H__

#include <cstdlib>
#include <cassert>
#include <vector>
#include <z3.h>

#define MAXVARS (1024 * 1024 * 16)
#define MAXFUNCS 2048
#define MAXDATA (1024 * 1024 * 32)
#define MAXARITYDATA (1024 * 16)
#define MAXSORTS 1024
#define MAXSUBSORTINGS 1024
#define MAXSUBST (1024 * 1024 * 8)

enum BuiltinSortType {
  bltnBool,
  bltnInt,
  bltnArray,
};

enum BuiltinFuncType {
  bltnAnd,
  bltnImplies,
  bltnOr,
  bltnNot,
  
  bltnTrue,
  bltnFalse,
  
  bltnNumeral,
  
  bltnLE,
  
  bltnPlus,
  bltnTimes,
  bltnDiv,
  bltnMod,
  bltnMinus,
  
  bltnEqInt,
  bltnEqBool,
  bltnEqArray,

  bltnForall,
  bltnExists,

  bltnIte,

  bltnSelect,
  bltnStore,
  bltnConstArray,

  bltnUD,

  bltnFreshConstant,
};

typedef unsigned int uint32;

typedef unsigned int uint;

typedef uint32 FastVar;   /* 0 .. MAXVARS - 1 */
typedef uint32 FastFunc;  /* 0 .. MAXFUNCS - 1 */
typedef uint32 FastTerm;  /* 0 .. MAXVARS - 1 and MAXVARS .. */
                          /* if >= MAXVARS, then it represents a pointer into termData */
typedef uint32 FastSort;  /* 0 .. MAXSORTS - 1 */

const uint32 MISSING_UELEM = 0xFFFFFFFF;

struct FastSubst {
  uint32 size;
  uint32 count;
  uint32 *data;

  FastSubst();
  FastSubst(const FastSubst &);
  FastSubst &operator=(const FastSubst &);
  ~FastSubst();
  void addToSubst(FastVar var, FastTerm term);
  void replaceConstWithVar(FastFunc c, FastVar);
  bool inDomain(FastVar var);
  FastTerm image(FastVar var);
  FastTerm applySubst(FastTerm term);
  void composeWith(FastVar v, FastTerm t);
  void applyInRange(FastVar v, FastTerm t);
};

FastSubst compose(FastSubst sigma1, FastSubst sigma2);

/*
  Initialization.
 */
void initFastTerm();
void initSorts();
void initFuncs();

/*
  Sorts.
 */
FastSort newArraySort(const char *name, FastSort domainSort, FastSort rangeSort);
FastSort newSort(const char *name);
bool existsSort(const char *name);
FastSort getSortByName(const char *name);

bool validFastSort(FastSort sort);

bool isBuiltinSort(FastSort sort);

BuiltinSortType getBuiltinSortType(FastSort sort);

FastSort fastBoolSort();
FastSort fastIntSort();
FastSort fastStateSort();

void newSubSort(FastSort subsort, FastSort supersort);

bool isSubSortTransitive(FastSort subsort, FastSort supersort);

const char *getSortName(FastSort sort);

/*
  Variables.
 */
FastVar newVar(const char *name, FastSort sort);
FastVar createFreshVariable(FastSort sort);
bool validFastVar(FastVar var);
const char *getVarName(FastVar var);
FastSort getVarSort(FastVar var);
bool eq_var(FastVar var1, FastVar var2);

bool existsVar(const char *name);
FastVar getVarByName(const char *name);

/*
  Function symbols.
*/
FastFunc newConst(const char *name, FastSort sort);
FastFunc newFunc(const char *name, FastSort resultSort, uint32 arity, FastSort *args);

FastFunc newInterpretedFunc(const char *name, FastSort resultSort, uint32 arity, FastSort *args);

FastFunc newACFunc(const char *name, FastSort sort);
FastFunc newACUFunc(const char *name, FastFunc uElem);
FastFunc getUnityElement(FastFunc func);
bool isFuncAC(FastFunc func);
bool validFastFunc(FastFunc func);

FastSort getArgSort(FastFunc func, uint arg);
const char *getFuncName(FastFunc);
uint getArity(FastFunc func);
FastSort getFuncSort(FastFunc func);

bool isBuiltinFunc(FastFunc func);
BuiltinFuncType getBuiltinFuncType(FastFunc func);

bool eq_func(FastFunc func1, FastFunc func2);

bool existsFunc(const char *name);
FastFunc getFuncByName(const char *name);

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

FastTerm replaceConstWithVar(FastTerm term, FastTerm c, FastVar v);

/*
  Builtins.
 */
FastTerm fastEq(FastTerm t1, FastTerm t2);
FastTerm fastEqInt(FastTerm t1, FastTerm t2);
FastTerm fastEqBool(FastTerm t1, FastTerm t2);
FastTerm fastAnd(FastTerm, FastTerm);
FastTerm fastAndVector(std::vector<FastTerm>);
FastTerm fastOrVector(std::vector<FastTerm>);
FastTerm fastImplies(FastTerm, FastTerm);
FastTerm fastExists(FastVar, FastTerm);
FastTerm fastForall(FastVar, FastTerm);
FastTerm fastImplies(FastTerm, FastTerm);
FastTerm fastOr(FastTerm, FastTerm);
FastTerm fastNot(FastTerm);
FastTerm fastLE(FastTerm t1, FastTerm t2);
FastTerm fastTrue();
FastTerm fastFalse();
FastTerm fastZero();
FastTerm fastOne();
FastTerm fastPlusVector(FastTerm, FastTerm);
FastTerm fastTimesVector(FastTerm, FastTerm);
FastTerm fastPlus(FastTerm, FastTerm);
FastTerm fastTimes(FastTerm, FastTerm);
FastTerm fastDiv(FastTerm, FastTerm);
FastTerm fastMod(FastTerm, FastTerm);
FastTerm fastPlusVector(std::vector<FastTerm> vt);
FastTerm fastTimesVector(std::vector<FastTerm> vt);

/*
Simplification
 */
FastTerm simplifyFast(FastTerm);
FastTerm simplify(FastTerm);

/*
  Substitutions.
*/

FastTerm applyUnitySubst(FastTerm term, FastVar v, FastTerm t);
size_t printSubst(FastSubst &subst, char *buffer, size_t size);

/*
  Helper functions.
*/

bool occurs(FastVar var, FastTerm term);
bool identifierTaken(const char *name);

/*
  Syntactic unification.
 */
std::vector<FastSubst> unify(FastTerm t1, FastTerm t2);

std::vector<FastSubst> match(FastTerm subject, FastTerm pattern);

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
void add_z3_assert(FastTerm term);
FastTerm unZ3(Z3_ast ast, FastSort sort, std::vector<FastVar> boundVars, Z3_context z3context);

/*
C++ wrappers.
 */
std::string toString(FastTerm term);
std::string toString(FastSubst subst);

#include <sstream>

void printToOss(FastTerm term, std::ostringstream &oss);
void varsOf(FastTerm term, std::vector<FastVar> &vars);
std::vector<FastVar> uniqueVars(FastTerm term);
void abortWithMessage(const std::string &error);

#endif