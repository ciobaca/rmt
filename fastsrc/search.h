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
  FastTerm rhs;
  FastSubst subst;
  FastTerm constraint;

  SmtSearchSolution(FastTerm iterm,
		    FastTerm rhs,
		    FastSubst subst,
		    FastTerm constraint) :
  iterm(iterm),
    rhs(rhs),
    subst(subst),
    constraint(constraint)
  {
  }
};

std::string toString(const SmtSearchSolution &solution);

std::vector<SmtSearchSolution> smtSearchRewriteSystem(const ConstrainedTerm &ct,
						      const RewriteSystem &rs,
						      uint32 minDepth, uint32 maxDepth);

std::vector<SmtSearchSolution> smtSearchRewriteSystem(FastTerm iterm, FastTerm iconstraint, const RewriteSystem &rs);

std::vector<SmtSearchSolution> smtSearchRewriteSystem(const ConstrainedTerm &ct, const RewriteSystem &rs);

std::vector<SmtSearchSolution> prune(const std::vector<SmtSearchSolution> &sols);

#endif
