#include "fastterm.h"
#include <stdio.h>
#include <cstring>
#include <assert.h>
#include <z3.h>
#include <stdlib.h>

void initFastTerm()
{
  initSorts();
  initFuncs();
}

uint32 termDataSize = 0;
uint32 termData[MAXDATA];

bool validFastVarTerm(FastTerm term)
{
  assert(validFastTerm(term));
  return isVariable(term);
}

bool validFastFuncTerm(FastTerm term)
{
  assert(validFastTerm(term));
  return isFuncTerm(term);
}

bool validFastTerm(FastTerm term)
{
  assert(/* 0 <= termDataSize && */termDataSize < MAXDATA);
  return (validFastVar(term)) ||
    (MAXVARS <= term && term < MAXVARS + termDataSize);
}

FastTerm newFuncTerm(FastFunc func, FastTerm *args)
{
  if (termDataSize + getArity(func) > MAXDATA) {
    fprintf(stderr, "Too much term data.\n");
    exit(-1);
  }
  uint32 result = termDataSize;
  termData[result] = func;
  termDataSize++;
  for (uint i = 0; i < getArity(func); ++i) {
    termData[result + i + 1] = args[i];
    termDataSize++;
  }
  assert(validFastTerm(result + MAXVARS));
  return result + MAXVARS;
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
    assert(/* 0 <= term && */term < MAXVARS);
    size_t len = strlen(getVarName(term));
    if (len <= size) {
      for (size_t i = 0; i < len; ++i) {
        buffer[i] = getVarName(term)[i];
      }
    }
    return len;
  } else {
    assert(isFuncTerm(term));
    assert(term >= MAXVARS);
    size_t index = term - MAXVARS;
    assert(/* 0 <= index && */ index < termDataSize);

    FastFunc func = termData[index];
    size_t len = strlen(getFuncName(func));
    size_t printedResult = 0;
    if (len <= size) {
      for (size_t i = 0; i < len; ++i) {
        buffer[i] = getFuncName(func)[i];
        printedResult++;
      }

      buffer = buffer + len;
      size = size - len;
      if (size >= 1 && getArity(func) > 0) {
        buffer[0] = '(';
        size--;
        printedResult++;
        buffer++;
      }
      for (uint i = 0; i < getArity(func); ++i) {
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
      if (size >= 1 && getArity(func) > 0) {
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

FastFunc getFunc(FastTerm term)
{
  assert(isFuncTerm(term));
  assert(term >= MAXVARS);
  uint32 index = term - MAXVARS;
  return termData[index];
}

FastTerm getArg(FastTerm term, uint arg)
{
  assert(validFastTerm(term));
  assert(validFastFuncTerm(term));
  assert(/* 0 <= arg && */arg < getArity(getFunc(term)));
  return termData[term - MAXVARS + arg + 1];
}

FastTerm *args(FastTerm term)
{
  assert(isFuncTerm(term));
  assert(term >= MAXVARS);
  uint32 index = term - MAXVARS;
  return &termData[index + 1];
}

bool occurs(FastVar var, FastTerm term)
{
  if (term == var) {
    return true;
  }
  if (isFuncTerm(term)) {
    FastFunc func = getFunc(term);
    for (uint i = 0; i < getArity(func); ++i) {
      if (occurs(var, getArg(term, i))) {
        return true;
      }
    }
    return false;
  }
  return false;
}

bool identifierTaken(const char *name)
{
  return existsSort(name) || existsVar(name) || existsFunc(name);
}

FastSort getSort(FastTerm term)
{
  assert(validFastTerm(term));
  if (isVariable(term)) {
    return getVarSort(term);
  } else {
    assert(validFastFuncTerm(term));
    return getFuncSort(getFunc(term));
  }
}

bool eq_term_list(FastTerm *tl1, FastTerm *tl2, uint count)
{
  for (uint i = 0; i < count; ++i) {
    if (!eq_term(tl1[i], tl2[i])) {
      return false;
    }
  }
  return 0;
}

bool eq_term(FastTerm t1, FastTerm t2)
{
  assert(validFastTerm(t1));
  assert(validFastTerm(t2));
  if (isFuncTerm(t1) && isFuncTerm(t2)) {
    FastFunc func1 = getFunc(t1);
    FastFunc func2 = getFunc(t2);
    if (!eq_func(func1, func2)) {
      return false;
    }
    FastTerm *args1 = args(t1);
    FastTerm *args2 = args(t2);
    return eq_term_list(args1, args2, getArity(func1));
  } else if (isVariable(t1) && isVariable(t2)) {
    return eq_var(t1, t2);
  } else {
    return false;
  }
}
