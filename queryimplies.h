#ifndef QUERYSMTIMPLIES_H__
#define QUERYSMTIMPLIES_H__

#include "query.h"
#include "constrainedterm.h"
#include <string>
#include <map>

struct QuerySmtImplies : public Query
{
  Term *term;
  ConstrainedTerm ct;

  QuerySmtImplies();
  
  virtual Query *create();
  
  virtual void parse(std::string &s, int &w);

  virtual void execute();
};


#endif
