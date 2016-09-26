#ifndef QUERYSMTPROVE_H__
#define QUERYSMTPROVE_H__

#include "query.h"
#include <string>
#include <map>

struct QuerySmtProve : public Query
{
  std::string rewriteSystemName;
  std::string circularitiesRewriteSystemName;

  QuerySmtProve();
  
  virtual Query *create();
  
  virtual void parse(std::string &s, int &w);
  
  virtual void execute();
};

#endif
