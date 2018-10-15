#include "constrainedterm.h"
#include "term.h"
#include "log.h"
#include "factories.h"
#include "z3driver.h"
#include <sstream>
#include <cassert>

using namespace std;

string ConstrainedTerm::toString()
{
  ostringstream oss;
  oss << term->toString();
  if (constraint) {
    oss << " /\\ ";
    oss << constraint->toString();
  }
  return oss.str();
}

string ConstrainedTerm::toPrettyString()
{
  ostringstream oss;
  oss << term->toPrettyString();
  if (constraint) {
    oss << " if ";
    oss << constraint->toPrettyString();
  }
  return oss.str();
}

vector<ConstrainedSolution> ConstrainedTerm::smtNarrowSearch(ConstrainedRewriteSystem &crs)
{
  Log(DEBUG7) << "ConstrainedTerm::smtNarrowSearch(ConstrainedRewriteSystem &crs) " << this->toString();
  vector<ConstrainedSolution> sols = term->normalizeFunctions()->smtNarrowSearchWdf(crs, constraint->normalizeFunctions());
  return sols;
}

void ConstrainedTerm::smtNarrowSearchHelper(ConstrainedRewriteSystem &crs,
							       int minDepth, int maxDepth, int depth,
							       vector<ConstrainedTerm> &result)
{
  if (minDepth <= depth && depth <= maxDepth) {
    result.push_back(*this);
  } 
  if (depth < maxDepth) {
    vector<ConstrainedSolution> sols = this->smtNarrowSearch(crs);
    for (int i = 0; i < (int)sols.size(); ++i) {
      ConstrainedTerm(sols[i].term->substitute(sols[i].subst)->substitute(sols[i].simplifyingSubst)->normalizeFunctions(),
		      sols[i].getFullConstraint(*this)->substitute(sols[i].subst)->substitute(sols[i].simplifyingSubst)->normalizeFunctions()).smtNarrowSearchHelper(crs, minDepth, maxDepth, depth + 1, result);
    }
  }
}

vector<ConstrainedTerm> ConstrainedTerm::smtNarrowSearch(ConstrainedRewriteSystem &crs, int minDepth, int maxDepth)
{
  assert(0 <= minDepth);
  assert(minDepth <= maxDepth);
  assert(maxDepth <= 99999999);

  vector<ConstrainedTerm> result;
  smtNarrowSearchHelper(crs, minDepth, maxDepth, 0, result);
  return result;
}

ConstrainedTerm ConstrainedTerm::normalize(RewriteSystem &rs)
{
  ConstrainedTerm ct(this->term->normalize(rs), this->constraint->normalize(rs));
  return ct;
}

vector<Variable *> ConstrainedTerm::vars()
{
  vector<Variable *> result;
  append(result, term->vars());
  append(result, constraint->vars());
  return result;
}

ConstrainedTerm ConstrainedTerm::substitute(Substitution &subst)
{
  ConstrainedTerm ct(term->substitute(subst), constraint->substitute(subst));
  return ct;
}

ConstrainedTerm ConstrainedTerm::fresh()
{
  vector<Variable *> myvars;
  append(myvars, term->vars());
  append(myvars, constraint->vars());

  map<Variable *, Variable *> renaming = freshRenaming(myvars);

  Substitution subst = createSubstitution(renaming);

  return this->substitute(subst);
}


// assume *this = <t1 if c1>
// assume goal = <t2 if c2>
// returns a constraint c such that
//
// forall X, c(X) /\ c1(X) -> exists Y, s.t. c2(X, Y) /\ t1(X) = t2(X,Y)
// 
// X are all variables in <t1 if c1>
// Y are all variables in <t2 if c2>
Term *ConstrainedTerm::whenImplies(ConstrainedTerm goal)
{
  ConstrainedTerm freshGoal = goal.fresh();
  vector<Variable *> vars = freshGoal.vars();
  Substitution subst;
  Term *constraint;
  if (this->term->unifyModuloTheories(freshGoal.term, subst, constraint)) {
    Term *lhsConstraint = this->constraint->substitute(subst);
    Term *rhsConstraint = freshGoal.constraint->substitute(subst);
    Term *resultingConstraint = bImplies(lhsConstraint, introduceExists(bAnd(constraint, rhsConstraint), vars));
    if (isSatisfiable(bNot(resultingConstraint)) == unsat) {
      return bTrue();
    }
    if (isSatisfiable(resultingConstraint) != unsat) {
      return resultingConstraint;
    }
  }
  return bFalse();
}

ConstrainedTerm ConstrainedTerm::normalizeFunctions()
{
  Term *newTerm = term, *newConstraint = constraint;
  if (term->hasDefinedFunctions) {
    RewriteSystem functionsRS = getRewriteSystem("functions");
    newTerm = term->normalize(functionsRS, false);
  }
  if (constraint->hasDefinedFunctions) {
    RewriteSystem functionsRS = getRewriteSystem("functions");
    newConstraint = constraint->normalize(functionsRS, false);
  }
  return ConstrainedTerm(newTerm, newConstraint);
}
