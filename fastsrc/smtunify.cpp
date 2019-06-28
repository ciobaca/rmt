#include "smtunify.h"
#include "helper.h"
#include "abstract.h"
#include "log.h"
#include "fastterm.h"
#include <sstream>
#include <iostream>

using namespace std;

extern FastFunc funcAnd;
extern bool hasEqFunc[MAXSORTS];
extern FastFunc eqFunc[MAXSORTS];

void extractConjunction(FastTerm constraint, vector<FastTerm> &result)
{
  if (isVariable(constraint)) {
    result.push_back(constraint);
  }
  assert(isFuncTerm(constraint));
  FastFunc func = getFunc(constraint);
  if (eq_func(func, funcAnd)) {
    assert(getArity(func) == 2);
    for (uint i = 0; i < getArity(func); ++i) {
      extractConjunction(getArg(constraint, i), result);
    }
  } else {
    result.push_back(constraint);
  }
}

void simplifySmtUnifySolution(SmtUnifySolution &solution)
{
  vector<FastTerm> constraints;
  extractConjunction(solution.constraint, constraints);

  FastSubst subst;
  for (uint i = 0; i < constraints.size(); ++i) {
    FastTerm constraint = constraints[i];
    FastFunc func = getFunc(constraint);
    if (getArity(func) == 2) {
      FastSort sort1 = getSort(getArg(constraint, 0));
      FastSort sort2 = getSort(getArg(constraint, 1));
      if (sort1 == sort2 && hasEqFunc[sort1] && eq_func(eqFunc[sort1], func)) {
	assert(getArity(func) == 2);
	FastTerm t1 = getArg(constraint, 0);
	FastTerm t2 = getArg(constraint, 1);
	if (isVariable(t2) && !subst.inDomain(t2)) {
	  subst.addToSubst(t2, t1);
	  solution.subst.addToSubst(t2, t1);
	} else if (isVariable(t1) && !subst.inDomain(t1)) {
	  subst.addToSubst(t1, t2);
	  solution.subst.addToSubst(t1, t2);
	}
      }
    }
  }
  solution.constraint = subst.applySubst(solution.constraint);
}

vector<SmtUnifySolution> smtUnify(FastTerm t1, FastTerm t2)
{
  LOG(DEBUG6, cerr << "smtUnify " << toString(t1) << " " << toString(t2));
  FastTerm t1Abstract;
  FastSubst sigma1;

  FastTerm t2Abstract;
  FastSubst sigma2;
  
  abstractTerm(t1, t1Abstract, sigma1);
  abstractTerm(t2, t2Abstract, sigma2);

  LOG(DEBUG6, cerr << "abstraction of " << toString(t1) << " = " << toString(t1Abstract) << "{" << toString(sigma1) << "}");
  LOG(DEBUG6, cerr << "abstraction of " << toString(t2) << " = " << toString(t2Abstract) << "{" << toString(sigma2) << "}");
  vector<FastSubst> solutions = unify(t1Abstract, t2Abstract);
  vector<SmtUnifySolution> result;
  LOG(DEBUG6, cerr << "unification produces " << solutions.size() << " solutions.");
  for (uint i = 0; i < solutions.size(); ++i) {
    LOG(DEBUG6, cerr << "solution " << (i + 1) << ":" << toString(solutions[i]));
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
	sigmaPrime.addToSubst(var, sigma2.applySubst(sigma1.applySubst(term)));
      }
    }
    SmtUnifySolution solution(sigmaPrime, constraint);
    LOG(DEBUG6, cerr << "smt-unify solution before simplification: " << toString(solution));
    simplifySmtUnifySolution(solution);
    LOG(DEBUG6, cerr << "smt-unify solution after  simplification: " << toString(solution));
    result.push_back(solution);
  }
  return result;
}

string toString(SmtUnifySolution solution)
{
  ostringstream oss;

  oss << toString(solution.subst) << " if " << toString(solution.constraint);

  return oss.str();
}
