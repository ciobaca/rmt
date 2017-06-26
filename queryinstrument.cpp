#include "queryinstrument.h"
#include "parse.h"
#include "factories.h"
#include "z3driver.h"
#include "log.h"
#include <iostream>
#include <string>
#include <map>
#include <cassert>

using namespace std;

QueryInstrument::QueryInstrument()
{
}

Query *QueryInstrument::create()
{
  return new QueryInstrument();
}

void QueryInstrument::parse(std::string &s, int &w)
{
  matchString(s, w, "instrument");
  skipWhiteSpace(s, w);
  rewriteSystemName = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  newSystemName = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  oldStateSortName = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  newStateSortName = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  protectFunctionName = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  protectVariableName = getIdentifier(s, w);
}

void QueryInstrument::execute()
{
  if (!existsCRewriteSystem(rewriteSystemName)) {
    Log(ERROR) << "Could not find constrained rewrite system " << rewriteSystemName << " (neigher regular or constrained)" << endl;
    return;
  }
  if (existsCRewriteSystem(newSystemName) || existsRewriteSystem(newSystemName)) {
    Log(ERROR) << "There already exists a (constrained) rewrite system with name " << newSystemName << "." << endl;
  }
  if (!sortExists(oldStateSortName)) {
    Log(ERROR) << "Sort of configurations " << newStateSortName << " does not exist." << endl;
    return;
  }
  if (sortExists(newStateSortName)) {
    Log(ERROR) << "Sort " << newStateSortName << " already exists." << endl;
    return;
  }
  if (getFunction(protectFunctionName)) {
    Log(ERROR) << "Function symbol" << protectFunctionName << " already exists." << endl;
    return;
  }
  if (getVariable(protectVariableName)) {
    Log(ERROR) << "Variable " << protectVariableName << " already exists." << endl;
    return;
  }

  createUninterpretedSort(newStateSortName);
  {
    vector <Sort*> arguments;
    arguments.push_back(getSort(newStateSortName));
    arguments.push_back(getIntSort());
    createUninterpretedFunction(protectFunctionName, arguments, getSort(oldStateSortName), false);
  }
  createVariable(protectVariableName, getIntSort());
  Term *leftSideProtection = getVarTerm(getVariable(protectVariableName));
  Term *rightSideProtection;
  {
    vector<Term*> arguments;
    arguments.push_back(leftSideProtection);
    arguments.push_back(getIntOneConstant());
    rightSideProtection = getFunTerm(getMinusFunction(), arguments);
  }
  Function *protectFuntion = getFunction(protectFunctionName);

  CRewriteSystem &rs = getCRewriteSystem(rewriteSystemName);
  CRewriteSystem nrs;
  for (const auto &it : rs) {
    Term *leftTerm = it.first.term, *leftConstraint = it.first.constraint, *rightTerm = it.second;

    vector<Term*> arguments;
    arguments.push_back(leftTerm);
    arguments.push_back(leftSideProtection);
    leftTerm = getFunTerm(protectFuntion, arguments);

    arguments.clear();
    arguments.push_back(rightTerm);
    arguments.push_back(rightSideProtection);
    rightTerm = getFunTerm(protectFuntion, arguments);

    nrs.addRule(ConstrainedTerm(leftTerm, leftConstraint), rightTerm);
  }

  putCRewriteSystem(newSystemName, nrs);

  CRewriteSystem &testrs = getCRewriteSystem(newSystemName);
  for (const auto &it : testrs) {
    cout << it.first.term->toString() << " /\\ " << it.first.constraint->toString() << " => " <<
      it.second->toString() << endl;
  }
}
