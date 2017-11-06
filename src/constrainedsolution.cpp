#include "constrainedsolution.h"
#include <sstream>
#include "substitution.h"
#include "term.h"
#include "constrainedterm.h"
#include "factories.h"

using namespace std;

ConstrainedSolution::ConstrainedSolution(Term *term, Term *constraint, Substitution subst, Term *lhsTerm) :
  term(term),
  constraint(constraint),
  lhsTerm(lhsTerm),
  subst(subst)
{
}

ConstrainedSolution::ConstrainedSolution(Term *term, Substitution subst, Term *lhsTerm) :
  term(term),
  constraint(bTrue()),
  lhsTerm(lhsTerm),
  subst(subst)
{
}

std::string ConstrainedSolution::toString() {
  std::ostringstream oss;
  oss << term->toString() << " if " << constraint->toString();
  return oss.str();
}

ConstrainedTerm ConstrainedSolution::getConstrainedTerm()
{
  return ConstrainedTerm(term, constraint);
}

Term *ConstrainedSolution::getFullConstraint(ConstrainedTerm startTerm)
{
  Term *result = constraint;
  for (Substitution::iterator it = simplifyingSubst.begin(); it != simplifyingSubst.end(); ++it) {
    if (term->hasVariable(it->first) || startTerm.term->hasVariable(it->first) ||
  	startTerm.constraint->hasVariable(it->first)) {
      result = bAnd(result, createEqualityConstraint(getVarTerm(it->first), it->second));
    }
  }
  return result;
}
