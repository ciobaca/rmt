#ifndef CONSTRAINEDREWRITE_H__
#define CONSTRAINEDREWRITE_H__

#include <vector>
#include <string>
#include "constrainedterm.h"

struct CRewriteSystem : public vector<pair<ConstrainedTerm, Term *> >
{
  void addRule(ConstrainedTerm l, Term *r);
  CRewriteSystem rename(string);
  CRewriteSystem fresh(vector<Variable *>);
  std::string toString();
};

#endif
