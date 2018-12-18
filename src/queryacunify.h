#ifndef QUERYACUNIFY_H__
#define QUERYACUNIFY_H__

#include "query.h"
#include "term.h"
#include "constrainedterm.h"
#include <string>
#include <map>

struct QueryACUnify : public Query {
  Term *t1;
  Term *t2;

  QueryACUnify();
  virtual Query *create();
  virtual void parse(std::string &s, int &w);
  virtual void execute();
};

#endif
