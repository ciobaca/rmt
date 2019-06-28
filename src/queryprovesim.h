#ifndef QUERYPROVESIM_H__
#define QUERYPROVESIM_H__

#include "query.h"
#include "constrainedterm.h"
#include "rewritesystem.h"
#include <string>
#include <map>
#include <vector>

struct proveSimulationExistsRight_arguments {
  ConstrainedTerm ct;
  bool progressRight;
  int depth;

  proveSimulationExistsRight_arguments(ConstrainedTerm ct, bool progressRight, int depth);
};


struct QueryProveSim : public Query {

  Z3_context context;

  std::vector<ConstrainedTerm> circularities;
  std::vector<ConstrainedTerm> base;
  
  RewriteSystem crsLeft;
  RewriteSystem crsRight;

  FastFunc pairFun;

  /* query parameters */
  int maxDepth;
  bool proveTotal;
  bool isBounded;

  /* holds information about which proofs failed */
  std::vector<int> failedCircularities;
  /* holds which circularities are assumed to be true */
  std::vector<bool> assumedCircularities;

  QueryProveSim();

  bool useDFS;

  virtual Query *create();

  virtual void parse(std::string &s, int &w);

  virtual void execute();

  //functions which help with execution
  bool proveSimulation(ConstrainedTerm ct, int depth);
  bool proveSimulationForallLeft(ConstrainedTerm ct, bool progressLeft, int depth);
  FastTerm proveSimulationExistsRight(proveSimulationExistsRight_arguments args, bool progressLeft);
  bool possibleLhsBase(FastTerm lhs);
  bool possibleRhsBase(FastTerm rhs);
  bool possibleLhsCircularity(FastTerm lhs);
  bool possibleRhsCircularity(FastTerm);
  FastTerm *proveBaseCase(ConstrainedTerm ct, bool progressLeft, bool progressRight, int depth);
  FastTerm *whenImpliesBase(ConstrainedTerm current);
  FastTerm *whenImpliesCircularity(ConstrainedTerm current);
  bool possibleCircularity(ConstrainedTerm ct);
  ConstrainedTerm pairC(FastTerm left, FastTerm right, FastTerm constraint);
  bool canApplyCircularities(bool progressLeft, bool progressRight);

  static void decomposeConstrainedTermEq(ConstrainedTerm ct, FastTerm &lhs, FastTerm &rhs);
};

#endif
