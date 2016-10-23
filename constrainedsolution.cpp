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
  //  oss << "subst: " << subst.toString() << endl;
  //  oss << "lhs term: " << lhsTerm->toString() << endl;
  //  oss << endl;
  return oss.str();
}

ConstrainedTerm ConstrainedSolution::getConstrainedTerm()
{
  return ConstrainedTerm(term, constraint);
}

