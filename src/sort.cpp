#include "sort.h"

Sort::Sort(string n) {
  name = n;
  hasInterpretation = false;
}

Sort::Sort(string n, string i) {
  name = n;
  hasInterpretation = true;
  if (i == "Bool") {
    interpretation = z3_bool();
  } else if (i == "Int") {
    interpretation = z3_int();
  } else {
    abortWithMessage("Unable to create interpreted sort " + i + " (unknown sort).");
  }
}

bool Sort::addSubSort(Sort *subSort) {
  if (this->hasInterpretation) {
    return 0; // cannot add subsorts to interpreted sorts
  }
  if (subSort->hasSubSortTR(this)) {
    return 0; // cannot add subsorting relation as it would create a cycle
  }
  subSorts.push_back(subSort);
  subSort->superSorts.push_back(this);
  return 1;
}

bool Sort::hasSubSortTR(Sort *sort) {
  if (this == sort) {
    return 1;
  }
  for (int i = 0; i < (int)subSorts.size(); ++i) {
    if (subSorts[i]->hasSubSortTR(sort)) {
      return 1;
    }
  }
  return 0;
}
