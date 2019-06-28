#include "defined.h"
#include <map>

using namespace std;

bool isFuncDefined[MAXFUNCS];
std::map<FastFunc, RewriteSystem> rsDefinition;
RewriteSystem rsDefinedCombined;

bool isDefinedFunction(FastFunc func)
{
  assert(validFastFunc(func));
  return isFuncDefined[func];
}

void makeFunctionDefined(FastFunc func, const RewriteSystem &rs)
{
  assert(validFastFunc(func));
  isFuncDefined[func] = true;
  rsDefinition[func] = rs;
  for (uint i = 0; i < rs.size(); ++i) {
    rsDefinedCombined.addRule(rs[i].first, rs[i].second);
  }
}

FastTerm compute(FastTerm term, Z3_context context)
{
  if (isVariable(term)) {
    return term;
  } else {
    assert(isFuncTerm(term));
    FastFunc func = getFunc(term);
    vector<FastTerm> newargs;
    for (uint i = 0; i < getArity(func); ++i) {
      newargs.push_back(compute(getArg(term, i), context));
    }
    if (isDefinedFunction(func)) {
      FastSubst how;
      FastTerm afterOneStep = rewriteTopMost(term, rsDefinition[func], how, context);
      if (!eq_term(afterOneStep, term)) {
	return compute(simplify(afterOneStep), context);
      } else {
	return term;
      }
    } else {
      return newFuncTerm(func, &newargs[0]);
    }
  }
}
