#ifndef QUERYPROVESIM2_H__
#define QUERYPROVESIM2_H__

#include "query.h"
#include "term.h"

struct QueryProveSim2 : public Query {

  int boundL, boundR;

  ConstrainedRewriteSystem crsLeft, crsRight;
  vector<ConstrainedPair> circularities, base;

  QueryProveSim2();
  
  virtual Query *create();

  virtual void parse(std::string &s, int &w);

  virtual void execute();

  Term *prove(ConstrainedPair PQ, int depth);
  Term *getBaseConstraint(ConstrainedPair PQ, vector<ConstrainedPair> &base, bool requireProgress);
};

#endif
