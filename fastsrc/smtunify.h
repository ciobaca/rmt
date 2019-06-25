#ifndef SMTUNIFY_H__
#define SMTUNIFY_H__

#include "fastterm.h"
#include <vector>
#include <string>

struct Solution
{
  FastSubst subst;
  FastTerm constraint;

  Solution(FastSubst subst, FastTerm constraint) :
    subst(subst), constraint(constraint)
  {
  }
};

std::vector<Solution> smtUnify(FastTerm t1, FastTerm t2);
std::string toString(Solution solution);

#endif
