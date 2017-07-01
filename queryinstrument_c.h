#ifndef QUERYINSTRUMENTC_H__
#define QUERYINSTRUMENTC_H__

#include "query.h"
#include "constrainedterm.h"
#include "function.h"
#include <string>
#include <map>

struct QueryInstrument_C : public Query
{
  QueryInstrument_C();
  
  virtual Query *create();
  
  virtual void parse(std::string &s, int &w);

  virtual void execute();

private:
  std::string rewriteSystemName, newSystemName, oldStateSortName, newStateSortName,
    protectFunctionName;
  vector<Term*> variants;
  Term *leftSideProtection, *rightSideProtection, *naturalNumberConstraint;
  Function *protectFunction;
  bool initialize();
  void addRuleFromOldRule(CRewriteSystem &nrs, Term *leftTerm, Term *leftConstraint, Term *rightTerm, int &variantIndex);
  void buildNewRewriteSystem();
};


#endif
