#include "smtunify.h"
#include "helper.h"
#include "abstract.h"
#include "log.h"
#include <sstream>
#include <iostream>

using namespace std;

vector<Solution> smtUnify(FastTerm t1, FastTerm t2)
{
  FastTerm t1Abstract;
  FastSubst sigma1;

  FastTerm t2Abstract;
  FastSubst sigma2;
  
  abstractTerm(t1, t1Abstract, sigma1);
  abstractTerm(t2, t2Abstract, sigma2);

  vector<FastSubst> solutions = unify(t1Abstract, t2Abstract);
  vector<Solution> result;
  for (uint i = 0; i < solutions.size(); ++i) {
    FastSubst sigma = solutions[i];
    FastSubst sigmaPrime;
    FastTerm constraint = fastTrue();
    for (uint j = 0; j < sigma.count; j += 2) {
      FastVar var = sigma.data[j];
      FastVar term = sigma.data[j + 1];

      if (isBuiltinSort(getSort(var))) {
	// term should also be a variable
	if (!isVariable(term)) {
	  cerr << "Internal error: got term where a variable was expected" << endl;
	  LOG(ERROR, cerr << "tried to smtunify " << toString(t1) << " and " << toString(t2));
	  assert(0);
	  exit(-1);
	} else {
	  assert(!(sigma1.inDomain(var) && sigma2.inDomain(var)));
	  assert(!(sigma1.inDomain(term) && sigma2.inDomain(term)));
	  constraint = fastAnd(constraint,
			      fastEq(
				     sigma2.applySubst(sigma1.applySubst(var)),
				     sigma2.applySubst(sigma1.applySubst(term)))
			      );
	}
      } else {
	sigmaPrime.addToSubst(var, term);
      }
    }
    result.push_back(Solution(sigmaPrime, constraint));
  }
  return result;
}

string toString(Solution solution)
{
  ostringstream oss;

  oss << toString(solution.subst) << " if " << toString(solution.constraint);

  return oss.str();
}
