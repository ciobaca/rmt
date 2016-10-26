#ifndef CONSTRAINEDSOLUTION_H__
#define CONSTRAINEDSOLUTION_H__

#include <string>
#include "substitution.h"

struct Term;

/*
  Represents a solution to a narrowing search question.

  The interpretation of a solution should be that "term" can be
  reached if "constraint /\ simplifyingSubst" is true.

  lhsTerm should the lhs of the rewrite rule that generated this
  solution.

  subst should be the mgu that was used to instantiate the lhs of the
  rule that was applied.
 */
struct ConstrainedTerm;

struct ConstrainedSolution
{
  Term *term;
  Term *constraint;
  Term *lhsTerm;
  Substitution subst;
  Substitution simplifyingSubst; // the part of the constraint that can be encoded as substitution

  ConstrainedSolution(Term *term, Term *constraint, Substitution subst, Term *lhsTerm);
  ConstrainedSolution(Term *term, Substitution subst, Term *lhsTerm);

  Term *getFullConstraint(ConstrainedTerm); // returns the full constraint, including the simplifyingSubstitution, assuming search started from the constrained term given as argument

  std::string toString();

  ConstrainedTerm getConstrainedTerm();
};

#endif
