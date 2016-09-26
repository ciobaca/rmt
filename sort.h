#ifndef SORT_H__
#define SORT_H__

#include <vector>
#include <string>

using namespace std;

struct Sort
{
  string name;
  bool hasInterpretation;
  string interpretation;
  vector<Sort *> subSorts;
  vector<Sort *> superSorts;

  Sort(string n) {
    name = n;
    hasInterpretation = false;
  }

  Sort(string n, string i) {
    name = n;
    hasInterpretation = true;
    interpretation = i;
  }

  bool addSubSort(Sort *subSort) {
    if (subSort->hasSubSortTR(this)) {
      return 0; // cannot add subsorting relation as it would create a cycle
    }
    subSorts.push_back(subSort);
    subSort->superSorts.push_back(this);
    return 1;
  }

  bool hasSubSortTR(Sort *sort) {
    if (this == sort) {
      return 1;
    }
    for (int i = 0; i < subSorts.size(); ++i) {
      if (subSorts[i]->hasSubSortTR(sort)) {
	return 1;
      }
    }
    return 0;
  }
};

#endif
