#include "fastterm.h"
#include <stdio.h>
#include <string.h>
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
  assert(0 <= varCount && varCount < MAXVARS);
  assert(0 <= termDataSize && termDataSize < MAXDATA);
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
    assert(0 <= term && term < MAXVARS);
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
    assert(0 <= index && index < termDataSize);

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
  assert(0 <= arg && arg < getArity(getFunc(term)));
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
    FastTerm *arguments = args(term);
    for (uint i = 0; i < getArity(func); ++i) {
      if (occurs(var, *arguments)) {
	return true;
      }
      arguments++;
    }
    return false;
  }
  return false;
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
