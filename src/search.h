#ifndef SEARCH_H__
#define SEARCH_H__

#include "search.h"
#include "constrainedterm.h"
#include "rewritesystem.h"
#include "constrainedterm.h"
#include <string>

struct SmtSearchSolution
{
  FastTerm iterm;
  FastTerm lhs;
  FastTerm ruleConstraint;
  FastTerm rhs;
  FastSubst subst;
  FastTerm constraint;

  SmtSearchSolution(FastTerm iterm,
		    FastTerm lhs,
		    FastTerm ruleConstraint,
		    FastTerm rhs,
		    FastSubst subst,
		    FastTerm constraint) :
  iterm(iterm),
    lhs(lhs),
    ruleConstraint(ruleConstraint),
    rhs(rhs),
    subst(subst),
    constraint(constraint)
  {
  }
};

std::string toString(const SmtSearchSolution &solution);
std::vector<SmtSearchSolution> prune(const std::vector<SmtSearchSolution> &sols,
  Z3_context context);
std::vector<SmtSearchSolution> smtSearchRewriteSystem(const ConstrainedTerm &ct,
						      const RewriteSystem &rs,
						      uint32 minDepth, uint32 maxDepth);

std::vector<SmtSearchSolution> smtSearchRewriteSystem(FastTerm iterm, FastTerm iconstraint, const RewriteSystem &rs);

std::vector<SmtSearchSolution> smtSearchRewriteSystem(const ConstrainedTerm &ct, const RewriteSystem &rs);

std::vector<SmtSearchSolution> prune(const std::vector<SmtSearchSolution> &sols);

FastTerm smtDefinedSimplify(const ConstrainedTerm &ct, const RewriteSystem &rs);

std::vector<ConstrainedTerm> solutionsToTerms(std::vector<SmtSearchSolution> &sols);

#endif
