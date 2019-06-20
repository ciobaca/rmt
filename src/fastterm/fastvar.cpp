#include "fastterm.h"
#include <cassert>
#include <string>

uint32 varCount = 0;
uint32 freshVarCount = 0;
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
  varNames[varCount] = strdup(name);
  if (varNames[varCount] == 0) {
    fprintf(stderr, "Not enough memory to duplicate C string (variable name).\n");
    exit(-1);
  }
  varSorts[varCount] = sort;
  FastVar result = varCount++;
  assert(validFastVar(result));
  return result;
}

FastVar createFreshVariable(FastSort sort) {
  char *name = new char[12]();
  name[0] = '_';
  int index = 1;
  for (char c : std::to_string(freshVarCount)) {
    name[index] = c;
    ++index;
  }
  ++freshVarCount;
  return newVar(name, sort);
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

bool existsVar(const char *name)
{
  for (uint32 i = 0; i < varCount; ++i) {
    if (strcmp(varNames[i], name) == 0) {
      return true;
    }
  }
  return false;
}

FastVar getVarByName(const char *name)
{
  assert(existsVar(name));
  for (uint32 i = 0; i < varCount; ++i) {
    if (strcmp(varNames[i], name) == 0) {
      assert(validFastVar(i));
      return i;
    }
  }
  assert(0);
  return 0;
}
