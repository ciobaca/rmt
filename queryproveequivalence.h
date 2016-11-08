#ifndef QUERYPROVEEQUIVALENCE_H__
#define QUERYPROVEEQUIVALENCE_H__

#include "query.h"
#include <string>
#include <vector>
#include "constrainedterm.h"

struct QueryProveEquivalence : public Query
{
  std::string lrsName;
  std::string rrsName;
  std::vector<ConstrainedTerm> circularities;
  std::vector<ConstrainedTerm> base;

  int maxDepth;
  int maxBranchingDepth;

  QueryProveEquivalence();
  
  virtual Query *create();
  
  virtual void parse(std::string &s, int &w);
  
  virtual void execute();
};

#endif
