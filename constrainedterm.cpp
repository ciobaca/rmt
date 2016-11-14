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
    oss << " if ";
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

vector<ConstrainedSolution> ConstrainedTerm::smtNarrowSearch(RewriteSystem &rs)
{
  Log(DEBUG7) << "ConstrainedTerm::smtNarrowSearch(RewriteSystem &rs) " << this->toString();
  vector<ConstrainedSolution> sols = term->smtNarrowSearch(rs, constraint);
  return sols;
}

vector<ConstrainedSolution> ConstrainedTerm::smtNarrowSearch(CRewriteSystem &crs)
{
  Log(DEBUG7) << "ConstrainedTerm::smtNarrowSearch(CRewriteSystem &crs) " << this->toString();
  vector<ConstrainedSolution> sols = term->smtNarrowSearch(crs, constraint);
  return sols;
}

void ConstrainedTerm::smtNarrowSearchHelper(RewriteSystem &rs,
							       int minDepth, int maxDepth, int depth,
							       vector<ConstrainedTerm> &result)
{
  if (minDepth <= depth && depth <= maxDepth) {
    result.push_back(*this);
  }
  if (depth < maxDepth) {
    vector<ConstrainedSolution> sols = this->smtNarrowSearch(rs);
    for (int i = 0; i < (int)sols.size(); ++i) {
      ConstrainedTerm(sols[i].term->substitute(sols[i].subst)->substitute(sols[i].simplifyingSubst),
		      sols[i].getFullConstraint(*this)->substitute(sols[i].subst)->substitute(sols[i].simplifyingSubst)).smtNarrowSearchHelper(rs, minDepth, maxDepth, depth + 1, result);
    }
  }
}

void ConstrainedTerm::smtNarrowSearchHelper(CRewriteSystem &crs,
							       int minDepth, int maxDepth, int depth,
							       vector<ConstrainedTerm> &result)
{
  if (minDepth <= depth && depth <= maxDepth) {
    result.push_back(*this);
  } 
  if (depth < maxDepth) {
    vector<ConstrainedSolution> sols = this->smtNarrowSearch(crs);
    for (int i = 0; i < (int)sols.size(); ++i) {
      ConstrainedTerm(sols[i].term->substitute(sols[i].subst)->substitute(sols[i].simplifyingSubst),
		      sols[i].getFullConstraint(*this)->substitute(sols[i].subst)->substitute(sols[i].simplifyingSubst)).smtNarrowSearchHelper(crs, minDepth, maxDepth, depth + 1, result);
    }
  }
}

vector<ConstrainedTerm> ConstrainedTerm::smtNarrowSearch(RewriteSystem &rs, int minDepth, int maxDepth)
{
  assert(0 <= minDepth);
  assert(minDepth <= maxDepth);
  assert(maxDepth <= 99999999);

  vector<ConstrainedTerm> result;
  smtNarrowSearchHelper(rs, minDepth, maxDepth, 0, result);
  return result;
}

vector<ConstrainedTerm> ConstrainedTerm::smtNarrowSearch(CRewriteSystem &crs, int minDepth, int maxDepth)
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
  // ct.term = this->term->normalize(rs);
  // if (this->constraint) {
  //   ct.constraint = this->constraint->normalize(rs);
  // } else {
  //   ct.constraint = 0;
  // }
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
