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
  std::string toString();
};

#endif
