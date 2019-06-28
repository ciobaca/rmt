#ifndef SMTUNIFY_H__
#define SMTUNIFY_H__

#include "fastterm.h"
#include <vector>
#include <string>

struct SmtUnifySolution
{
  FastSubst subst;
  FastTerm constraint;

  SmtUnifySolution(FastSubst subst, FastTerm constraint) :
    subst(subst), constraint(constraint)
  {
  }
};

std::vector<SmtUnifySolution> smtUnify(FastTerm t1, FastTerm t2);
std::string toString(SmtUnifySolution solution);

#endif
