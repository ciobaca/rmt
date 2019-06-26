#include "fastterm.h"
#include <cassert>
#include <sstream>
#include <cstring>
#include <iostream>

bool funcIsBuiltin[MAXFUNCS];
BuiltinFuncType builtinFunc[MAXFUNCS];
int builtinFuncExtra[MAXFUNCS]; // if builtinFunc[i] == bltnNumeral, then
                                // builtinFuncExtra[i] in { -256, ..., 256 } is the numeral

FastFunc funcAnd;
FastFunc funcOr;
FastFunc funcNot;
FastFunc funcTrue;
FastFunc funcFalse;
FastFunc funcLE;
FastFunc funcPlus;
FastFunc funcTimes;
FastFunc funcMinus;
FastFunc funcEqInt;
FastFunc funcEqBool;

bool hasEqFunc[MAXSORTS];
FastFunc eqFunc[MAXSORTS];

FastTerm termTrue;
FastTerm termFalse;

bool isAC[MAXFUNCS];
FastFunc uElem[MAXFUNCS];

uint32 funcCount = 0;
uint32 arities[MAXFUNCS]; // number of arguments
uint32 arityIndex[MAXFUNCS]; // where in arityData the argument sorts are
FastSort resultSorts[MAXFUNCS]; // the result sort
FastSort arityData[MAXARITYDATA]; // at arityData[arityIndex[f]] start the
                                  // arities[f] sorts of the arguments
uint32 arityDataIndex = 0;
const char *funcNames[MAXFUNCS];

bool isBuiltinFunc(FastFunc func)
{
  assert(validFastFunc(func));
  return funcIsBuiltin[func];
}

BuiltinFuncType getBuiltinFuncType(FastFunc func)
{
  assert(validFastFunc(func));
  assert(isBuiltinFunc(func));
  return builtinFunc[func];
}

void initFuncs()
{
  funcTrue = newConst("true", fastBoolSort());
  funcIsBuiltin[funcTrue] = true;
  builtinFunc[funcTrue] = bltnTrue;

  funcFalse = newConst("false", fastBoolSort());
  funcIsBuiltin[funcFalse] = false;
  builtinFunc[funcFalse] = bltnFalse;

  FastSort args[16];
  args[0] = fastBoolSort();
  args[1] = fastBoolSort();

  funcAnd = newFunc("band", fastBoolSort(), 2, args);
  funcIsBuiltin[funcAnd] = true;
  builtinFunc[funcAnd] = bltnAnd;

  funcOr = newFunc("bor", fastBoolSort(), 2, args);
  funcIsBuiltin[funcOr] = true;
  builtinFunc[funcOr] = bltnOr;

  funcNot = newFunc("bnot", fastBoolSort(), 1, args);
  funcIsBuiltin[funcNot] = true;
  builtinFunc[funcNot] = bltnNot;

  funcEqBool = newFunc("beq", fastBoolSort(), 2, args);
  funcIsBuiltin[funcEqBool] = true;
  builtinFunc[funcEqBool] = bltnEqBool;
  hasEqFunc[fastBoolSort()] = true;
  eqFunc[fastBoolSort()] = funcEqBool;

  termTrue = newFuncTerm(funcTrue, args);
  termFalse = newFuncTerm(funcFalse, args);

  for (int i = -256; i <= 256; ++i) {
    std::ostringstream oss;
    oss << i;
    FastFunc funci = newConst(oss.str().c_str(), fastIntSort());
    funcIsBuiltin[funci] = true;
    builtinFunc[funci] = bltnNumeral;
    builtinFuncExtra[funci] = i;
  }

  args[0] = fastIntSort();
  args[1] = fastIntSort();

  funcLE = newFunc("mle", fastBoolSort(), 2, args);
  funcIsBuiltin[funcLE] = true;
  builtinFunc[funcLE] = bltnLE;

  funcPlus = newFunc("mplus", fastIntSort(), 2, args);
  funcIsBuiltin[funcPlus] = true;
  builtinFunc[funcPlus] = bltnPlus;

  funcTimes = newFunc("mtimes", fastIntSort(), 2, args);
  funcIsBuiltin[funcTimes] = true;
  builtinFunc[funcTimes] = bltnTimes;

  funcMinus = newFunc("mminus", fastIntSort(), 2, args);
  funcIsBuiltin[funcMinus] = true;
  builtinFunc[funcMinus] = bltnMinus;

  funcEqInt = newFunc("mequals", fastBoolSort(), 2, args);
  funcIsBuiltin[funcEqInt] = true;
  builtinFunc[funcEqInt] = bltnEqInt;

  hasEqFunc[fastIntSort()] = true;
  eqFunc[fastIntSort()] = funcEqInt;
}

FastTerm fastEq(FastTerm t1, FastTerm t2)
{
  FastSort sort1 = getSort(t1);
  FastSort sort2 = getSort(t2);
  if (sort1 != sort2) {
    std::cerr << "Internal error: tried to add equality between terms of different sorts." << std::endl;
    exit(-1);
  }
  if (!hasEqFunc[sort1]) {
    std::cerr << "Internal error: tried to add equality between terms of sort that does not feature equality." << std::endl;
    exit(-1);
  }
  if (eq_term(t1, t2)) {
    return fastTrue();
  }
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(eqFunc[sort1], args);
}

FastTerm fastEqInt(FastTerm t1, FastTerm t2)
{
  if (eq_term(t1, t2)) {
    return fastTrue();
  }
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcEqInt, args);
}

FastTerm fastEqBool(FastTerm t1, FastTerm t2)
{
  if (eq_term(t1, t2)) {
    return fastTrue();
  }
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcEqBool, args);
}

FastTerm fastAnd(FastTerm t1, FastTerm t2)
{
  if (t1 == fastFalse()) {
    return fastFalse();
  }
  if (t2 == fastFalse()) {
    return fastFalse();
  }
  if (t1 == fastTrue()) {
    return t2;
  }
  if (t2 == fastTrue()) {
    return t1;
  }
  if (eq_term(t1, t2)) {
    return t1;
  }
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcAnd, args);
}

FastTerm fastOr(FastTerm t1, FastTerm t2)
{
  if (t1 == fastTrue()) {
    return fastTrue();
  }
  if (t2 == fastTrue()) {
    return fastTrue();
  }
  if (t1 == fastFalse()) {
    return t2;
  }
  if (t2 == fastFalse()) {
    return t1;
  }
  if (eq_term(t1, t2)) {
    return t1;
  }
  FastTerm args[4];
  args[0] = t1;
  args[1] = t2;
  return newFuncTerm(funcOr, args);
}

FastTerm fastNot(FastTerm t1)
{
  FastTerm args[4];
  args[0] = t1;
  return newFuncTerm(funcNot, args);
}

FastTerm fastTrue()
{
  return termTrue;
}

FastTerm fastFalse()
{
  return termFalse;
}

bool validFastFunc(FastFunc func)
{
  assert(/* 0 <= funcCount && */ funcCount < MAXFUNCS);
  return /* 0 <= func && */ func < funcCount;
}

FastFunc newConst(const char *name, FastSort sort)
{
  if (funcCount == MAXFUNCS) {
    fprintf(stderr, "Too many functions.\n");
    exit(-1);
  }
  arities[funcCount] = 0;
  funcNames[funcCount] = strdup(name);
  if (funcNames[funcCount] == 0) {
    fprintf(stderr, "Not enough memory to duplicate C string (function name).\n");
    exit(-1);
  }
  arityIndex[funcCount] = 0; // not to be used (as arity is 0)
  resultSorts[funcCount] = sort;
  uElem[funcCount] = MISSING_UELEM;
  isAC[funcCount] = false;
  FastFunc result = funcCount++;
  assert(validFastFunc(result));
  return result;
}

FastFunc newFunc(const char *name, FastSort resultSort, uint32 argCount, FastSort *args)
{
  if (funcCount == MAXFUNCS) {
    fprintf(stderr, "Too many functions.\n");
    exit(-1);
  }
  if (arityDataIndex + argCount > MAXARITYDATA) {
    fprintf(stderr, "Too many function arguments.\n");
    exit(-1);
  }
  arities[funcCount] = argCount;
  funcNames[funcCount] = strdup(name);
  if (funcNames[funcCount] == 0) {
    fprintf(stderr, "Not enough memory to duplicate C string (function name).\n");
    exit(-1);
  }
  resultSorts[funcCount] = resultSort;
  arityIndex[funcCount] = arityDataIndex;
  for (uint i = 0; i < argCount; ++i) {
    arityData[arityDataIndex++] = args[i];
  }
  uElem[funcCount] = MISSING_UELEM;
  isAC[funcCount] = false;
  FastFunc result = funcCount++;
  assert(validFastFunc(result));
  return result;
}

FastFunc newACFunc(const char *name, FastSort sort) {
  if (funcCount == MAXFUNCS) {
    fprintf(stderr, "Too many functions.\n");
    exit(-1);
  }
  if (arityDataIndex + 2 > MAXARITYDATA) {
    fprintf(stderr, "Too many function arguments.\n");
    exit(-1);
  }
  arities[funcCount] = 2;
  funcNames[funcCount] = strdup(name);
  if (funcNames[funcCount] == 0) {
    fprintf(stderr, "Not enough memory to duplicate C string (function name).\n");
    exit(-1);
  }
  resultSorts[funcCount] = sort;
  arityIndex[funcCount] = arityDataIndex;
  for (uint i = 0; i < 2; ++i) {
    arityData[arityDataIndex++] = sort;
  }
  uElem[funcCount] = MISSING_UELEM;
  isAC[funcCount] = true;
  FastFunc result = funcCount++;
  assert(validFastFunc(result));
  return result;
}

FastFunc newACUFunc(const char *name, FastFunc uElem) {
  if (funcCount == MAXFUNCS) {
    fprintf(stderr, "Too many functions.\n");
    exit(-1);
  }
  if (arityDataIndex + 2 > MAXARITYDATA) {
    fprintf(stderr, "Too many function arguments.\n");
    exit(-1);
  }
  arities[funcCount] = 2;
  funcNames[funcCount] = strdup(name);
  if (funcNames[funcCount] == 0) {
    fprintf(stderr, "Not enough memory to duplicate C string (function name).\n");
    exit(-1);
  }
  resultSorts[funcCount] = getFuncSort(uElem);
  arityIndex[funcCount] = arityDataIndex;
  for (uint i = 0; i < 2; ++i) {
    arityData[arityDataIndex++] = getFuncSort(uElem);
  }
  ::uElem[funcCount] = uElem;
  isAC[funcCount] = true;
  FastFunc result = funcCount++;
  assert(validFastFunc(result));
  return result;
}

FastFunc getUnityElement(FastFunc func) {
  assert(validFastFunc(func));
  return uElem[func]; 
}

bool isFuncAC(FastFunc func) {
  assert(validFastFunc(func));
  return isAC[func];
}

FastSort getFuncSort(FastFunc func)
{
  assert(validFastFunc(func));
  return resultSorts[func];
}

FastSort getArgSort(FastFunc func, uint arg)
{
  assert(validFastFunc(func));
  return arityData[arityIndex[func] + arg];
}

uint getArity(FastFunc func)
{
  assert(validFastFunc(func));
  return arities[func];
}

const char *getFuncName(FastTerm func)
{
  assert(validFastFunc(func));
  return funcNames[func];
}

bool eq_func(FastFunc func1, FastFunc func2)
{
  assert(validFastFunc(func1));
  assert(validFastFunc(func2));
  return func1 == func2;
}

bool existsFunc(const char *name)
{
  for (uint32 i = 0; i < funcCount; ++i) {
    if (strcmp(funcNames[i], name) == 0) {
      return true;
    }
  }
  return false;
}

FastFunc getFuncByName(const char *name)
{
  assert(existsFunc(name));
  for (uint32 i = 0; i < funcCount; ++i) {
    if (strcmp(funcNames[i], name) == 0) {
      assert(validFastFunc(i));
      return i;
    }
  }
  assert(0);
  return 0;
}
