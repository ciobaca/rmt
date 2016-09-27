#ifndef QUERYSMTRUN_H__
#define QUERYSMTRUN_H__

#include "query.h"
#include "constrainedterm.h"
#include <string>
#include <map>

struct QuerySmtRun : public Query
{
  std::string rewriteSystemName;
  Term *term;

  QuerySmtRun();
  
  virtual Query *create();

  virtual void parse(std::string &s, int &w);

  virtual void execute();
};


#endif
