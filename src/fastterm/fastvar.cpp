#include "fastterm.h"
#include <cassert>

uint32 varCount = 0;
FastSort varSorts[MAXVARS];
const char *varNames[MAXVARS];

bool validFastVar(FastVar var)
{
  assert(0 <= varCount && varCount < MAXVARS);
  return /* 0 <= var && */ var < varCount;
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
 
const char *getVarName(FastVar var)
{
  assert(validFastVar(var));
  return varNames[var];
}

FastSort getVarSort(FastVar var)
{
  assert(validFastVar(var));
  return varSorts[var];
}
