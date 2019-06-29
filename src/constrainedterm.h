#ifndef CONSTRAINEDTERM_H__
#define CONSTRAINEDTERM_H__

#include <string>
#include <vector>
#include "fastterm.h"

struct ConstrainedTerm
{
  FastTerm term;
  FastTerm constraint;

  ConstrainedTerm(FastTerm term, FastTerm constraint) : term(term), constraint(constraint)
  {
  }

  /* std::vector<Function *> getDefinedFunctions(); */

  /* std::string toString(); */
  /* std::string toPrettyString(); */
  
  /* ConstrainedRewriteSystem getDefinedFunctionsSystem(int printDepth = 0); */
  
  /* vector<ConstrainedSolution> smtNarrowDefinedSearch(int printDepth = 0); */
  /* std::vector<ConstrainedTerm> smtNarrowDefinedSearch(int minDepth, int maxDepth, int printDepth = 0); */
  /* vector<ConstrainedSolution> smtRewriteDefined(int printDepth); */
  /* vector<ConstrainedSolution> smtRewriteDefined(int printDepth, ConstrainedRewriteSystem rs); */

  /* std::vector<ConstrainedSolution> smtNarrowSearch(ConstrainedRewriteSystem &crs); */

  /* std::vector<ConstrainedTerm> smtNarrowSearch(ConstrainedRewriteSystem &crs, int minDepth, int maxDepth); */

  /* void smtNarrowSearchHelper(ConstrainedRewriteSystem &crs, int minDepth, int maxDepth, int depth, vector<ConstrainedTerm> &result); */

  /* ConstrainedTerm normalize(RewriteSystem &rs); */

  /* std::vector<Variable *> vars(); */
  /* ConstrainedTerm substitute(Substitution &); */

  /* //  ConstrainedTerm normalizeFunctions(); */

  FastTerm whenImplies(ConstrainedTerm goal);

  ConstrainedTerm fresh();
};

std::string toString(const ConstrainedTerm &ct);

ConstrainedTerm simplify(const ConstrainedTerm &ct);

//std::string toPrettyString(const ConstrainedTerm &ct);

/* vector<ConstrainedTerm> solutionsToSuccessors(vector<ConstrainedSolution> &solutions); */

/* struct ConstrainedPair { */
/*   Term *lhs, *rhs, *constraint; */
/*   ConstrainedPair() : lhs(NULL), rhs(NULL), constraint(NULL) {} */
/*   ConstrainedPair(Term *lhs, Term *rhs, Term *constraint) */
/*     : lhs(lhs), rhs(rhs), constraint(constraint) {} */
/*   std::string toString(); */
/* }; */

#endif
