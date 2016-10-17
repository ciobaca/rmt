#ifndef QUERYSMTPROVE_H__
#define QUERYSMTPROVE_H__

#include "query.h"
#include "constrainedterm.h"
#include <string>
#include <map>

struct RewriteSystem;
struct CRewriteSystem;
struct Term;

struct QuerySmtProve : public Query
{
  std::string rewriteSystemName;
  std::string circularitiesRewriteSystemName;

  int maxDepth;
  int maxBranchingDepth;

  QuerySmtProve();
  
  virtual Query *create();
  
  virtual void parse(std::string &s, int &w);
  
  virtual void execute();

  Term *proveByImplication(ConstrainedTerm, Term *, RewriteSystem &, CRewriteSystem &, int);
  Term *proveByCircularities(ConstrainedTerm, Term *, RewriteSystem &, CRewriteSystem &, int, bool, int);
  Term *proveByRewrite(ConstrainedTerm, Term *, RewriteSystem &, CRewriteSystem &, int, bool, int);
  void prove(ConstrainedTerm, Term *, RewriteSystem &, CRewriteSystem &, bool, int = 0, int = 0);
};

#endif
