#ifndef CONSTRAINEDTERM_H__
#define CONSTRAINEDTERM_H__

#include <string>
#include "rewrite.h"

struct Term;

struct ConstrainedTerm
{
  Term *term;
  Term *constraint;

  std::string toString();

  std::vector<ConstrainedTerm> smtNarrowSearch(RewriteSystem &rs);
  ConstrainedTerm normalize(RewriteSystem &rs);
};

#endif
