#ifndef QUERYPROVEREACHABILITY_H__
#define QUERYPROVEREACHABILITY_H__

#include "query.h"
#include "constrainedterm.h"
#include "constrainedrewrite.h"
#include <string>
#include <map>
#include <vector>

struct RewriteSystem;
struct Term;

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
  Term *rhs;
  Reason reason;

  ProofObligation(ConstrainedTerm lhs, Term *rhs, Reason reason) :
    lhs(lhs), rhs(rhs), reason(reason)
  {
  }
};

struct QueryProveReachability : public Query
{
  std::string rewriteSystemName;
  std::string circularitiesRewriteSystemName;

  int maxDepth;
  int maxBranchingDepth;

  std::vector<ProofObligation> unproven;

  QueryProveReachability();
  
  virtual Query *create();
  
  virtual void parse(std::string &s, int &w);
  
  virtual void execute();

  Term *proveByImplication(ConstrainedTerm, Term *, RewriteSystem &, CRewriteSystem &, int);
  Term *proveByCircularities(ConstrainedTerm, Term *, RewriteSystem &, CRewriteSystem &, int, bool, int);
  Term *proveByRewrite(ConstrainedTerm, Term *, RewriteSystem &, CRewriteSystem &, int, bool, int);
  void prove(ConstrainedTerm, Term *, RewriteSystem &, CRewriteSystem &, bool, int = 0, int = 0);

  Term *proveByImplicationCRS(ConstrainedTerm, Term *, CRewriteSystem &, CRewriteSystem &, int);
  Term *proveByCircularitiesCRS(ConstrainedTerm, Term *, CRewriteSystem &, CRewriteSystem &, int, bool, int);
  Term *proveByRewriteCRS(ConstrainedTerm, Term *, CRewriteSystem &, CRewriteSystem &, int, bool, int);
  void proveCRS(ConstrainedTerm, Term *, CRewriteSystem &, CRewriteSystem &, bool, int = 0, int = 0);
};

#endif
