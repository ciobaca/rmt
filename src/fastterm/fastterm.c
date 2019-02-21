#include "fastterm.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

uint32 sortCount = 0;
const char *sortNames[MAXSORTS];

FastSort subsortings[MAXSUBSORTINGS * 2];
uint32 subsortingCount = 0;

uint32 varCount = 0;
FastSort varSorts[MAXVARS];
const char *varNames[MAXVARS];

uint32 funcCount = 0;
uint32 arities[MAXFUNCS]; // number of arguments
uint32 arityIndex[MAXFUNCS]; // where in arityData the argument sorts are
FastSort resultSorts[MAXFUNCS]; // the result sort
FastSort arityData[MAXARITYDATA]; // at arityData[arityIndex[f]] start the
                                  // arities[f] sorts of the arguments
uint32 arityDataIndex = 0;
const char *funcNames[MAXFUNCS];

uint32 termDataSize = 0;
uint32 termData[MAXDATA];

uint32 substDataSize = 0;
uint32 substData[MAXSUBST];

bool validFastVar(FastVar var)
{
  assert(0 <= varCount && varCount < MAXVARS);
  return 0 <= var && var < varCount;
}

bool validFastFunc(FastFunc func)
{
  assert(0 <= funcCount && funcCount < MAXFUNCS);
  return 0 <= func && func < funcCount;
}

bool validFastTerm(FastTerm term)
{
  assert(0 <= varCount && varCount < MAXVARS);
  assert(0 <= termDataSize && termDataSize < MAXDATA);
  return (0 <= term & term < varCount) ||
    (MAXVARS <= term && term < MAXVARS + termDataSize);
}

bool validFastSort(FastSort sort)
{
  assert(0 <= sortCount && sortCount < MAXSORTS);
  return 0 <= sort && sort < sortCount;
}

FastSort newSort(const char *name)
{
  if (sortCount == MAXSORTS) {
    fprintf(stderr, "Too many sorts.\n");
    exit(-1);
  }
  sortNames[sortCount] = name;
  FastSort result = sortCount++;
  assert(validFastSort(result));
  return result;
}

void newSubsorting(FastSort subsort, FastSort supersort)
{
  if (subsortingCount == MAXSUBSORTINGS) {
    fprintf(stderr, "Too many subsortings.\n");
    exit(-1);
  }
  subsortings[subsortingCount * 2] = subsort;
  subsortings[subsortingCount * 2 + 1] = supersort;
  subsortingCount++;
}

FastVar newVar(const char *name, FastSort sort)
{
  if (varCount == MAXVARS) {
    fprintf(stderr, "Too many variables.\n");
    exit(-1);
  }
  varNames[varCount] = name;
  varSorts[varCount] = sort;
  FastVar result = varCount++;
  assert(validFastVar(result));
  return result;
}

FastFunc newConst(const char *name, FastSort sort)
{
  if (funcCount == MAXFUNCS) {
    fprintf(stderr, "Too many functions.\n");
    exit(-1);
  }
  arities[funcCount] = 0;
  funcNames[funcCount] = name;
  arityIndex[funcCount] = 0; // not to be used (as arity is 0)
  resultSorts[funcCount] = sort;
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
  funcNames[funcCount] = name;
  resultSorts[funcCount] = resultSort;
  arityIndex[funcCount] = arityDataIndex;
  for (int i = 0; i < argCount; ++i) {
    arityData[arityDataIndex++] = args[i];
  }
  FastFunc result = funcCount++;
  assert(validFastFunc(result));
  return result;
}

FastTerm newFuncTerm(FastFunc func, FastTerm *args)
{
  if (termDataSize + arities[func] > MAXDATA) {
    fprintf(stderr, "Too much term data.\n");
    exit(-1);
  }
  uint32 result = termDataSize + MAXVARS;
  termData[termDataSize++] = func;
  for (int i = 0; i < arities[func]; ++i) {
    termData[termDataSize++] = args[i];
  }
  assert(validFastTerm(result));
  return result;
}

bool isVariable(FastTerm term)
{
  assert(validFastTerm(term));
  return validFastVar(term);
}

bool isFuncTerm(FastTerm term)
{
  assert(validFastTerm(term));
  return MAXVARS <= term && term < MAXVARS + termDataSize;
}

size_t printTermRec(FastTerm term, char *buffer, size_t size)
{
  if (isVariable(term)) {
    assert(0 <= term && term < MAXVARS);
    size_t len = strlen(varNames[term]);
    if (len <= size) {
      for (size_t i = 0; i < len; ++i) {
	buffer[i] = varNames[term][i];
      }
    }
    return len;
  } else {
    assert(isFuncTerm(term));
    assert(term >= MAXVARS);
    size_t index = term - MAXVARS;
    assert(0 <= index && index < termDataSize);

    FastFunc func = termData[index];
    size_t len = strlen(funcNames[func]);
    size_t printedResult = 0;
    if (len <= size) {
      for (size_t i = 0; i < len; ++i) {
	buffer[i] = funcNames[func][i];
	printedResult++;
      }

      buffer = buffer + len;
      size = size - len;
      if (size >= 1 && arities[func] > 0) {
	buffer[0] = '(';
	size--;
	printedResult++;
	buffer++;
      }
      for (int i = 0; i < arities[func]; ++i) {
	if (size >= 1 && i > 0) {
	  buffer[0] = ',';
	  size--;
	  printedResult++;
	  buffer++;
	}
	size_t printed = printTermRec(termData[index + i + 1], buffer, size);
	buffer = buffer + printed;
	size = size - printed;
	printedResult += printed;
      }
      if (size >= 1 && arities[func] > 0) {
	buffer[0] = ')';
	size--;
	printedResult++;
	buffer++;
      }
    }
    return printedResult;
  }
}

size_t printTerm(FastTerm term, char *buffer, size_t size)
{
  assert(size > 0);
  size_t printed = printTermRec(term, buffer, size);
  if (printed < size) {
    buffer[printed] = 0;
    return printed;
  } else {
    buffer[size - 1] = 0;
    return size - 1;
  }
}

FastSubst newSubst()
{
  if (substDataSize >= MAXSUBST) {
    fprintf(stderr, "Too much substitution data.\n");
    exit(-1);
  }
  FastSubst result = substDataSize;
  substData[substDataSize++] = 0;
  return result;
}

void addToSubst(FastSubst subst, FastVar var, FastTerm term)
{
  if (substDataSize + 1 < MAXSUBST) {
    substData[subst]++; // add binding
    substData[substDataSize++] = var;
    substData[substDataSize++] = term;
  } else {
    fprintf(stderr, "Too much substitution data.\n");
    exit(-1);
  }
}

inline FastFunc funcSymbol(FastTerm term)
{
  assert(isFuncTerm(term));
  assert(term >= MAXVARS);
  uint32 index = term - MAXVARS;
  return termData[index];
}

inline FastTerm *args(FastTerm term)
{
  assert(isFuncTerm(term));
  assert(term >= MAXVARS);
  uint32 index = term - MAXVARS;
  return &termData[index + 1];
}

bool inRange(FastVar var, FastSubst subst)
{
  uint32 index = subst + 1;
  uint32 size = substData[subst];
  for (int i = 0; i < size; i++) {
    if (substData[index] == var) {
      return true;
    }
    index += 2;
  }
  return false;
}

FastTerm range(FastVar var, FastSubst subst)
{
  assert(inRange(var, subst));
  uint32 index = subst + 1;
  uint32 size = substData[subst];
  for (int i = 0; i < size; i++) {
    if (substData[index] == var) {
      return substData[index + 1];
    }
    index += 2;
  }
  assert(0);
  return 0;
}

FastTerm applySubst(FastTerm term, FastSubst subst)
{
  if (isVariable(term)) {
    if (inRange(term, subst)) {
      return range(term, subst);
    }
    return term;
  } else {
    assert(isFuncTerm(term));
    FastFunc func = funcSymbol(term);
    FastTerm result = newFuncTerm(func, args(term));
    FastTerm *arguments = args(result);
    for (int i = 0; i < arities[func]; ++i) {
      *arguments = applySubst(*arguments, subst);
      arguments++;
    }
    return result;
  }
}

bool unifyHelper(FastTerm, FastTerm, FastSubst);

bool unifyHelperList(FastTerm *tl1, FastTerm *tl2, uint32 count, FastSubst subst)
{
  for (int i = 0; i < count; ++i) {
    if (!unifyHelper(tl1[i], tl2[i], subst)) {
      return false;
    }
  }
  return true;
}

bool unifyHelper(FastTerm t1, FastTerm t2, FastSubst subst)
{
  t1 = applySubst(t1, subst);
  t2 = applySubst(t2, subst);
  if (isFuncTerm(t1) && isFuncTerm(t2)) {
    if (funcSymbol(t1) != funcSymbol(t2)) {
      return false;
    }
    return unifyHelperList(args(t1), args(t2), arities[funcSymbol(t1)], subst);
  } else {
    if (isFuncTerm(t1)) {
      FastTerm temp = t1;
      t1 = t2;
      t2 = temp;
    }
    assert(isVariable(t1));
    addToSubst(subst, t1, t2);
    return true;
  }
}

bool unify(FastTerm t1, FastTerm t2, FastSubst *result)
{
  *result = newSubst();
  return unifyHelper(t1, t2, *result);
}

size_t printSubst(FastSubst subst, char *buffer, size_t size)
{
  size_t result = 0;
  if (size >= 1) {
    buffer[0] = '{';
    size--;
    result++;
    buffer++;
  }
  if (size >= 1) {
    buffer[0] = ' ';
    size--;
    result++;
    buffer++;
  }
  uint32 count = substData[subst];
  for (int i = 0; i < count; ++i) {
    if (i > 0) {
      if (size >= 1) {
	buffer[0] = ',';
	size--;
	result++;
	buffer++;
      }
      if (size >= 1) {
	buffer[0] = ' ';
	size--;
	result++;
	buffer++;
      }
    }
    assert(validFastTerm(substData[subst + 2 * i + 1]));
    assert(isVariable(substData[subst + 2 * i + 1]));
    uint32 printed = printTerm(substData[subst + 2 * i + 1], buffer, size);
    size -= printed;
    result += size;
    if (size >= 1) {
      buffer[0] = '-';
      size--;
      result++;
      buffer++;
    }
    if (size >= 1) {
      buffer[0] = '>';
      size--;
      result++;
      buffer++;
    }
    assert(validFastTerm(substData[subst + 2 * i + 2]));
    printed = printTerm(substData[subst + 2 * i + 2], buffer, size);
    size -= printed;
    result += size;
  }
  if (size >= 1) {
    buffer[0] = ' ';
    size--;
    result++;
    buffer++;
  }
  if (size >= 1) {
    buffer[0] = '}';
    size--;
    result++;
    buffer++;
  }
  return result;
}
