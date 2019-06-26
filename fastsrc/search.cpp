#include "search.h"
#include "constrainedterm.h"
#include "log.h"
#include "abstract.h"
#include "smtunify.h"
#include "helper.h"
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

vector<SmtSearchSolution> smtSearchRewriteSystem(const ConstrainedTerm &ct,
    const RewriteSystem &rs, uint32 minDepth, uint32 maxDepth)
{
  // if (minDepth == 1 && maxDepth == 1) {
  //   return smtSearchRewriteSystem(ct, rs);
  // } else {
    vector<SmtSearchSolution> result;
    vector<SmtSearchSolution> current;
    current.push_back(SmtSearchSolution(ct.term, ct.term, FastSubst(), ct.constraint));
    for (uint32 level = 0; level <= maxDepth; ++level) {
      LOG(DEBUG3, cerr << "Level " << level);
      for (int i = 0; i < (int)current.size(); ++i) {
	LOG(DEBUG3, cerr << "Level " << level);
	LOG(DEBUG3, cerr << "    Solution #" << i + 1 << ":");
	LOG(DEBUG3, cerr << "    " <<  toString(current[i]));
      }

      if (level >= minDepth) {
	result.insert(result.end(), current.begin(), current.end());
      }
      vector<SmtSearchSolution> next;
      for (uint32 i = 0; i < current.size(); ++i) {
	SmtSearchSolution sol = current[i];
	vector<SmtSearchSolution> succs = smtSearchRewriteSystem(sol.subst.applySubst(sol.rhs), sol.constraint, rs);
	for (uint32 j = 0; j < succs.size(); ++j) {
	  next.push_back(SmtSearchSolution(sol.iterm, succs[j].rhs, compose(sol.subst, succs[j].subst), succs[j].constraint));
	}
      }
      current = next;
    }
    return result;
    //  }
}

string toString(const SmtSearchSolution &solution)
{
  ostringstream oss;

  oss << "(" << toString(solution.iterm) << " => " << toString(solution.rhs) << ")";
  oss << toString(solution.subst) << " if " << toString(solution.constraint);

  return oss.str();
}

void smtSearchRewriteRuleInternal(FastTerm cterm, FastTerm iterm, FastTerm iconstraint,
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
      smtSearchRewriteRuleInternal(nt, iterm, iconstraint, lhs, rhs, constraint, solutions);
    }
    vector<SmtUnifySolution> topMostSols = smtUnify(cterm, lhs);
    for (uint i = 0; i < topMostSols.size(); ++i) {
      SmtUnifySolution sol = topMostSols[i];
      solutions.push_back(SmtSearchSolution(iterm, rhs, sol.subst,
					    fastAnd(fastAnd(
							    sol.subst.applySubst(iconstraint),
							    sol.subst.applySubst(constraint)), sol.constraint)));
    }
  }
}

extern FastFunc funcAnd;
extern bool hasEqFunc[MAXSORTS];
extern FastFunc eqFunc[MAXSORTS];

void extractEqualities(FastTerm constraint, vector<pair<FastVar, FastTerm>> &result, const vector<FastVar> &varsRule)
{
  if (isVariable(constraint)) {
    return;
  }
  assert(isFuncTerm(constraint));
  FastFunc func = getFunc(constraint);
  if (eq_func(func, funcAnd)) {
    assert(getArity(func) == 2);
    for (uint i = 0; i < getArity(func); ++i) {
      extractEqualities(getArg(constraint, i), result, varsRule);
    }
  } else if (getArity(func) == 2) {
    FastSort sort1 = getSort(getArg(constraint, 0));
    FastSort sort2 = getSort(getArg(constraint, 1));
    if (sort1 == sort2 && hasEqFunc[sort1] && eq_func(eqFunc[sort1], func)) {
      assert(getArity(func) == 2);
      FastTerm t1 = getArg(constraint, 0);
      FastTerm t2 = getArg(constraint, 1);
      if (isVariable(t1) && getVarName(t1)[0] == '_') {
	result.push_back(make_pair(t1, t2));
      } else if (isVariable(t2) && getVarName(t2)[0] == '_') {
	result.push_back(make_pair(t2, t1));
      } else if (isVariable(t2) && contains(varsRule, t2)) {
	result.push_back(make_pair(t2, t1));
      } else if (isVariable(t1) && contains(varsRule, t1)) {
	result.push_back(make_pair(t1, t2));
      }
    }
  }
}

void smtSearchRewriteRule(FastTerm cterm, FastTerm iterm, FastTerm iconstraint,
	       FastTerm lhs, FastTerm rhs, FastTerm constraint,
	       vector<SmtSearchSolution> &solutions)
{
  /* STEP 1: create fresh instance of rewrite rule */
  vector<FastVar> varsRule;
  vector<FastVar> toRename;

  varsOf(lhs, varsRule);
  varsOf(rhs, varsRule);
  varsOf(constraint, varsRule);

  std::sort(varsRule.begin(), varsRule.begin());
  auto it = std::unique(varsRule.begin(), varsRule.end());
  varsRule.resize(std::distance(varsRule.begin(), it));
  for (uint i = 0; i < varsRule.size(); ++i) {
    if (occurs(varsRule[i], iterm) || occurs(varsRule[i], iconstraint)) {
      toRename.push_back(varsRule[i]);
    }
  }

  FastSubst subst;
  for (uint i = 0; i < toRename.size(); ++i) {
    FastVar var = createFreshVariable(getVarSort(toRename[i]));
    //    freshVars.push_back(var);
    subst.addToSubst(toRename[i], var);
  }
  lhs = subst.applySubst(lhs);
  rhs = subst.applySubst(rhs);
  constraint = subst.applySubst(constraint);

  /* STEP 2: perform narrowing */
  smtSearchRewriteRuleInternal(cterm, iterm, iconstraint, lhs, rhs, constraint, solutions);

  /* STEP 3: try to simplify constraint */
  for (uint i = 0; i < solutions.size(); ++i) {
    SmtSearchSolution sol = solutions[i];
    vector<pair<FastVar, FastTerm>> equalities;
    extractEqualities(sol.constraint, equalities, varsRule);

    for (uint j = 0; j < equalities.size(); ++j) {
      sol.iterm = applyUnitySubst(sol.iterm, equalities[j].first, equalities[j].second);
      sol.rhs = applyUnitySubst(sol.rhs, equalities[j].first, equalities[j].second);
      sol.subst.applyInRange(equalities[j].first, equalities[j].second);
      sol.constraint = simplify(applyUnitySubst(sol.constraint, equalities[j].first, equalities[j].second));
    }
    solutions[i] = sol;
  }
}

vector<SmtSearchSolution> smtSearchRewriteSystem(FastTerm iterm,
  FastTerm iconstraint, const RewriteSystem &rs)
{
  vector<SmtSearchSolution> result;
  for (uint i = 0; i < rs.size(); ++i) {
    FastTerm lhs = rs[i].first.term;
    FastTerm rhs = rs[i].second;

    smtSearchRewriteRule(iterm, iterm, iconstraint, lhs, rhs, rs[i].first.constraint, result);
  }
  return result;
}

vector<SmtSearchSolution> smtSearchRewriteSystem(const ConstrainedTerm &ct,
						 const RewriteSystem &rs)
{
  return smtSearchRewriteSystem(ct.term, ct.constraint, rs);
}

vector<SmtSearchSolution> prune(const vector<SmtSearchSolution> &sols)
{
  vector<SmtSearchSolution> result;
  Z3_context context = init_z3_context();
  for (uint i = 0; i < sols.size(); ++i) {
    if (z3_sat_check(context, sols[i].constraint) != Z3_L_FALSE) {
      result.push_back(sols[i]);
    }
  }
  return result;
}
