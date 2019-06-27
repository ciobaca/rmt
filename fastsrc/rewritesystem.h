#ifndef REWRITESYSTEM_H__
#define REWRITESYSTEM_H__

#include <vector>
#include <string>
#include "constrainedterm.h"

struct RewriteSystem : public std::vector<std::pair<ConstrainedTerm, FastTerm> >
{
  RewriteSystem() {}
  void addRule(ConstrainedTerm l, FastTerm r);
  RewriteSystem rename(std::string);
  RewriteSystem fresh();
};

FastTerm rewriteTopMost(FastTerm term, const RewriteSystem &rs, FastSubst &how, Z3_context context);
FastTerm rewriteTopMost(FastTerm term, const ConstrainedTerm &lhs, const FastTerm &rhs, FastSubst &how, Z3_context context);

#endif
