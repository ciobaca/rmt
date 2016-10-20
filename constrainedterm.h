#ifndef CONSTRAINEDTERM_H__
#define CONSTRAINEDTERM_H__

#include <string>
#include <vector>
#include "substitution.h"
#include "rewrite.h"
#include "constrainedsolution.h"

struct Term;

struct CRewriteSystem;

struct ConstrainedTerm
{
  Term *term;
  Term *constraint;

ConstrainedTerm(Term *term, Term *constraint) : term(term), constraint(constraint)
  {
  }

  std::string toString();
  std::string toPrettyString();

  std::vector<ConstrainedSolution> smtNarrowSearch(RewriteSystem &rs);
  std::vector<ConstrainedSolution> smtNarrowSearch(CRewriteSystem &crs);

  std::vector<ConstrainedTerm> smtNarrowSearch(RewriteSystem &rs, int minDepth, int maxDepth);
  std::vector<ConstrainedTerm> smtNarrowSearch(CRewriteSystem &crs, int minDepth, int maxDepth);

  void smtNarrowSearchHelper(RewriteSystem &rs, int minDepth, int maxDepth, int depth, vector<ConstrainedTerm> &result);
  void smtNarrowSearchHelper(CRewriteSystem &crs, int minDepth, int maxDepth, int depth, vector<ConstrainedTerm> &result);

  ConstrainedTerm normalize(RewriteSystem &rs);

  std::vector<Variable *> vars();
  ConstrainedTerm substitute(Substitution &);
};

#endif
