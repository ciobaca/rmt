#ifndef QUERYSMTNARROWSEARCH_H__
#define QUERYSMTNARROWSEARCH_H__

#include "query.h"
#include "constrainedterm.h"
#include <string>
#include <map>

struct QuerySmtNarrowSearch : public Query
{
  std::string rewriteSystemName;
  ConstrainedTerm ct;

  QuerySmtNarrowSearch();
  
  virtual Query *create();
  
  virtual void parse(std::string &s, int &w);

  virtual void execute();
};

#endif
