#include "abstract.h"
#include <vector>

using namespace std;

void abstractTerm(FastTerm term, FastTerm &resultTerm, FastSubst &abstractingSubst)
{
  if (isVariable(term)) {
    resultTerm = term;
  } else {
    assert(isFuncTerm(term));
    FastFunc func = getFunc(term);
    if (isBuiltinFunc(func)) {
      FastSort sort = getFuncSort(func);
      FastTerm newVar = createFreshVariable(sort);
      resultTerm = newVar;
      abstractingSubst.addToSubst(newVar, term);
    } else {
      vector<FastTerm> terms(getArity(func), 0);
      for (uint i = 0; i < getArity(func); ++i) {
	abstractTerm(getArg(term, i), terms[i], abstractingSubst);
      }
      resultTerm = newFuncTerm(func, &terms[0]);
    }
  }
}
