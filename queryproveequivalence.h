#ifndef QUERYPROVEEQUIVALENCE_H__
#define QUERYPROVEEQUIVALENCE_H__

#include "query.h"
#include <string>
#include <vector>
#include "constrainedterm.h"
#include "constrainedrewrite.h"

struct QueryProveEquivalence : public Query
{
  std::string lrsName;
  std::string rrsName;
  std::vector<ConstrainedTerm> circularities;
  std::vector<ConstrainedTerm> base;

  CRewriteSystem crsLeft;
  CRewriteSystem crsRight;

  int maxDepth;
  int maxBranchingDepth;

  QueryProveEquivalence();
  
  virtual Query *create();
  
  virtual void parse(std::string &s, int &w);
  
  virtual void execute();

  Term *whenImpliesBase(ConstrainedTerm current);
  bool possibleLhsBase(Term *);
  bool possibleRhsBase(Term *);
  void proveEquivalenceForallLeft(ConstrainedTerm ct, bool progress, int depth);
  void proveEquivalenceExistsRight(ConstrainedTerm ct, bool progress, int depth);
  void proveEquivalence(ConstrainedTerm ct, bool progress, int depth);
};

#endif
