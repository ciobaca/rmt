#ifndef QUERYINSTRUMENT_H__
#define QUERYINSTRUMENT_H__

#include "query.h"
#include "constrainedterm.h"
#include <string>
#include <map>

struct QueryInstrument : public Query
{
  std::string rewriteSystemName, newSystemName, oldStateSortName, newStateSortName,
    protectFunctionName, protectVariableName;

  QueryInstrument();
  
  virtual Query *create();
  
  virtual void parse(std::string &s, int &w);

  virtual void execute();
};


#endif
