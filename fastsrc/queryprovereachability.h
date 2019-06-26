#ifndef QUERYPROVEREACHABILITY_H__
#define QUERYPROVEREACHABILITY_H__

#include "query.h"
#include "constrainedterm.h"
#include "rewritesystem.h"
#include <string>
#include <map>
#include <vector>

enum Reason
{
  DepthLimit,
  BranchingLimit,
  Completeness
};

std::string stringFromReason(Reason);

struct ProofObligation
{
  ConstrainedTerm lhs;
  FastTerm rhs;
  Reason reason;

  ProofObligation(ConstrainedTerm lhs, FastTerm rhs, Reason reason) :
    lhs(lhs), rhs(rhs), reason(reason)
  {
  }
};

struct QueryProveReachability : public Query
{
  std::string rewriteSystemName;
  std::string circularitiesRewriteSystemName;

  Z3_context context;
  
  int maxDepth;
  int maxBranchingDepth;

  std::vector<ProofObligation> unproven;

  QueryProveReachability();
  
  virtual Query *create();
  
  virtual void parse(std::string &s, int &w);
  
  virtual void execute();

  FastTerm proveByImplication(ConstrainedTerm, FastTerm, RewriteSystem &, RewriteSystem &, int);
  FastTerm proveByCircularities(ConstrainedTerm, FastTerm, RewriteSystem &, RewriteSystem &, int, bool, int);
  FastTerm proveByRewrite(ConstrainedTerm, FastTerm, RewriteSystem &, RewriteSystem &, int, bool, int);
  void prove(ConstrainedTerm, FastTerm, RewriteSystem &, RewriteSystem &, bool, int = 0, int = 0);
};

#endif
