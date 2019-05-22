#include "fastterm.h"
#include <cassert>

uint32 sortCount = 0;
const char *sortNames[MAXSORTS];

bool sortIsBuiltin[MAXSORTS];
BuiltinSortType builtinSortType[MAXSORTS];

FastSort sortBool;
FastSort sortInt;
FastSort sortState;

FastSort subsortings[MAXSUBSORTINGS * 2];
uint32 subsortingCount = 0;

void initSorts()
{
  for (uint i = 0; i < MAXSORTS; ++i) {
    sortIsBuiltin[i] = false;
  }

  sortBool = newSort("Bool");
  sortIsBuiltin[sortBool] = true;
  builtinSortType[sortBool] = bltnBool;

  sortInt = newSort("Int");
  sortIsBuiltin[sortInt] = true;
  builtinSortType[sortInt] = bltnInt;
  
  sortState = newSort("State");
  sortIsBuiltin[sortState] = true;
  builtinSortType[sortState] = bltnState;
}

bool isBuiltinSort(FastSort sort)
{
  assert(validFastSort(sort));
  return sortIsBuiltin[sort];
}

FastSort fastBoolSort()
{
  return sortBool;
}

FastSort fastIntSort()
{
  return sortInt;
}

FastSort fastStateSort() {
  return sortState;
}

bool validFastSort(FastSort sort)
{
  assert(0 <= sortCount && sortCount < MAXSORTS);
  return /* 0 <= sort && */ sort < sortCount;
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

const char *getSortName(FastSort sort)
{
  assert(validFastSort(sort));
  return sortNames[sort];
}

BuiltinSortType getBuiltinSortType(FastSort sort)
{
  assert(validFastSort(sort));
  return builtinSortType[sort];
}
