#ifndef CONSTRAINEDTERM_H__
#define CONSTRAINEDTERM_H__

#include <string>
#include <vector>
#include "substitution.h"
#include "rewritesystem.h"
#include "constrainedsolution.h"

struct Term;

struct ConstrainedRewriteSystem;

struct ConstrainedTerm
{
  Term *term;
  Term *constraint;

  ConstrainedTerm(Term *term, Term *constraint) : term(term), constraint(constraint)
  {
  }

  std::string toString();
  std::string toPrettyString();

  std::vector<ConstrainedSolution> smtNarrowSearch(ConstrainedRewriteSystem &crs);

  std::vector<ConstrainedTerm> smtNarrowSearch(ConstrainedRewriteSystem &crs, int minDepth, int maxDepth);

  void smtNarrowSearchHelper(ConstrainedRewriteSystem &crs, int minDepth, int maxDepth, int depth, vector<ConstrainedTerm> &result);

  ConstrainedTerm normalize(RewriteSystem &rs);

  std::vector<Variable *> vars();
  ConstrainedTerm substitute(Substitution &);

  ConstrainedTerm normalizeFunctions();

  Term *whenImplies(ConstrainedTerm goal);

  ConstrainedTerm fresh();
};

#endif
