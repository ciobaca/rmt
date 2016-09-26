#ifndef QUERYSMTSATISFIABILITY_H__
#define QUERYSMTSATISFIABILITY_H__

#include "term.h"
#include "query.h"
#include <string>
#include <map>

struct QuerySmtSatisfiability : public Query
{
  Term *constraint;
  QuerySmtSatisfiability();
  
  virtual Query *create();
  
  virtual void parse(std::string &s, int &w);

  virtual void execute();
};

#endif
