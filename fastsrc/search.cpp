#include "search.h"
#include "constrainedterm.h"
#include "log.h"
#include "abstract.h"
#include "smtunify.h"
#include "helper.h"
#include <sstream>

using namespace std;

vector<SmtSearchSolution> smtSearchRewriteSystem(const ConstrainedTerm &ct,
    const RewriteSystem &rs, uint32 minDepth, uint32 maxDepth)
{
  if (minDepth == 1 && maxDepth == 1) {
    return smtSearchRewriteSystem(ct, rs);
  } else {
    cerr << "Not yet implemented." << endl;
    exit(-1);
  }
}

string toString(const SmtSearchSolution &solution)
{
  ostringstream oss;

  oss << "(" << toString(solution.iterm) << " => " << toString(solution.rhs) << ")";
  oss << toString(solution.subst) << " if " << toString(solution.constraint);

  return oss.str();
}

void smtSearchRewriteRule(FastTerm cterm, FastTerm iterm,
	       FastTerm lhs, FastTerm rhs, FastTerm constraint,
	       vector<SmtSearchSolution> &solutions)
{
  if (isVariable(cterm)) {
    // do not narrow in variables
  } else {
    assert(isFuncTerm(cterm));
    FastFunc func = getFunc(cterm);
    for (uint i = 0; i < getArity(func); ++i) {
      FastTerm nt = getArg(cterm, i);
      smtSearchRewriteRule(nt, iterm, lhs, rhs, constraint, solutions);
    }
    vector<SmtUnifySolution> topMostSols = smtUnify(cterm, lhs);
    for (uint i = 0; i < topMostSols.size(); ++i) {
      SmtUnifySolution sol = topMostSols[i];
      solutions.push_back(SmtSearchSolution(iterm, rhs, sol.subst,
	    fastAnd(sol.subst.applySubst(constraint), sol.constraint)));
    }
  }
}

vector<SmtSearchSolution> smtSearchRewriteSystem(FastTerm iterm,
  FastTerm iconstraint, const RewriteSystem &rs)
{
  vector<SmtSearchSolution> result;
  for (uint i = 0; i < rs.size(); ++i) {
    FastTerm lhs = rs[i].first.term;
    FastTerm rhs = rs[i].second;
    FastTerm constraint = fastAnd(iconstraint, rs[i].first.constraint);

    smtSearchRewriteRule(iterm, iterm, lhs, rhs, constraint, result);
  }
  return result;
}

vector<SmtSearchSolution> smtSearchRewriteSystem(const ConstrainedTerm &ct,
						 const RewriteSystem &rs)
{
  vector<SmtSearchSolution> result;
  for (uint i = 0; i < rs.size(); ++i) {
    FastTerm lhs = rs[i].first.term;
    FastTerm rhs = rs[i].second;
    FastTerm constraint = fastAnd(ct.constraint, rs[i].first.constraint);
    
    smtSearchRewriteRule(ct.term, ct.term, lhs, rhs, constraint, result);
  }
  return result;
}

// vector<SmtSearchSolution> prune(Z3_context context, vector<SmtSearchSolution> sols)
// {
//   for (uint i = 0; i < sols.size(); ++i) {
//     if (sols[i].constraint
//   }
// }
