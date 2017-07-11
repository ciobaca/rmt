#ifndef CONSTRAINEDREWRITE_H__
#define CONSTRAINEDREWRITE_H__

#include <vector>
#include <string>
#include "constrainedterm.h"

struct ConstrainedRewriteSystem : public vector<pair<ConstrainedTerm, Term *> >
{
  void addRule(ConstrainedTerm l, Term *r);
  ConstrainedRewriteSystem rename(string);
  ConstrainedRewriteSystem fresh(vector<Variable *>);
  std::string toString();
};

#endif
