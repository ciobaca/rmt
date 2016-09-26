#ifndef QUERYSMTUNIFY_H__
#define QUERYSMTUNIFY_H__

#include "query.h"
#include "term.h"
#include "constrainedterm.h"
#include <string>
#include <map>

struct QuerySmtUnify : public Query
{
  Term *term;
  ConstrainedTerm ct;

  QuerySmtUnify();
  virtual Query *create();
  virtual void parse(std::string &s, int &w);
  virtual void execute();
};

#endif
