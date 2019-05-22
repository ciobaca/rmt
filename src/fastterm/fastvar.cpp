#include "fastterm.h"
#include <cassert>
#include <string>

uint32 varCount = 0;
uint32 fresVarCount = 0;
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

FastVar createFreshVariable(FastSort sort) {
  return newVar(("_" + std::to_string(varCount++)).c_str(), sort);
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

bool eq_var(FastVar var1, FastVar var2)
{
  assert(validVariable(var1));
  assert(validVariable(var2));
  return var1 == var2;
}
