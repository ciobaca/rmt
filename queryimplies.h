#ifndef QUERYIMPLIES_H__
#define QUERYIMPLIES_H__

#include "query.h"
#include "constrainedterm.h"
#include <string>
#include <map>

struct QueryImplies : public Query
{
  Term *term;
  ConstrainedTerm ct;

  QueryImplies();
  
  virtual Query *create();
  
  virtual void parse(std::string &s, int &w);

  virtual void execute();
};


#endif
