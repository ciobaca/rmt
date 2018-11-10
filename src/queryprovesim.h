#ifndef QUERYPROVESIM_H__
#define QUERYPROVESIM_H__

#include "query.h"
#include <string>
#include <vector>
#include "constrainedrewritesystem.h"

struct proveSimulationExistsRight_arguments {
  ConstrainedTerm ct;
  bool progressRight;
  int depth;

  proveSimulationExistsRight_arguments(ConstrainedTerm ct, bool progressRight, int depth);
};

struct QueryProveSim : public Query {
  std::vector<ConstrainedTerm> circularities;
  std::vector<ConstrainedTerm> base;

  ConstrainedRewriteSystem crsLeft;
  ConstrainedRewriteSystem crsRight;

  Function *pairFun;

  int maxDepth;

  QueryProveSim();

  virtual Query *create();

  virtual void parse(std::string &s, int &w);

  virtual void execute();

  //functions which help with execution
  bool proveSimulation(ConstrainedTerm ct, bool progressLeft, bool progressRight, int depth);
  bool proveSimulationForallLeft(ConstrainedTerm ct, bool progressLeft, bool progressRight, int depth);
  Term *proveSimulationExistsRight(proveSimulationExistsRight_arguments args, bool progressLeft);
  bool possibleLhsBase(Term *lhs);
  bool possibleRhsBase(Term *rhs);
  bool possibleLhsCircularity(Term *lhs);
  bool possibleRhsCircularity(Term *);
  Term *proveBaseCase(ConstrainedTerm ct, bool progressLeft, bool progressRight, int depth);
  Term *whenImpliesBase(ConstrainedTerm current);
  Term *whenImpliesCircularity(ConstrainedTerm current);
  bool possibleCircularity(ConstrainedTerm ct);
  ConstrainedTerm pairC(Term *left, Term *right, Term *constraint);

  static void decomposeConstrainedTermEq(ConstrainedTerm ct, Term *&lhs, Term *&rhs);
};

#endif