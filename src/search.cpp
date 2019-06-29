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
  vector<SmtSearchSolution> result;
  vector<SmtSearchSolution> current;
  current.push_back(SmtSearchSolution(ct.term, fastFalse(), fastTrue(), ct.term, FastSubst(), ct.constraint));
  for (uint32 level = 0; level <= maxDepth; ++level) {
    LOG(DEBUG4, cerr << "Level " << level);
    for (int i = 0; i < (int)current.size(); ++i) {
      LOG(DEBUG4, cerr << "Level " << level);
      LOG(DEBUG4, cerr << "    Solution #" << i + 1 << ":");
      LOG(DEBUG4, cerr << "    " <<  toString(current[i]));
    }

    if (level >= minDepth) {
      result.insert(result.end(), current.begin(), current.end());
    }
    vector<SmtSearchSolution> next;
    for (uint32 i = 0; i < current.size(); ++i) {
      SmtSearchSolution sol = current[i];
      vector<SmtSearchSolution> succs = smtSearchRewriteSystem(sol.subst.applySubst(sol.rhs), sol.constraint, rs);
      for (uint32 j = 0; j < succs.size(); ++j) {
	SmtSearchSolution solp(simplify(sol.iterm), simplify(succs[j].lhs),
			       simplify(succs[j].ruleConstraint), simplify(succs[j].rhs),
			       compose(sol.subst, succs[j].subst), simplify(succs[j].constraint));
	FastSubst newsubst;
	for (uint i = 0; i < solp.subst.count; i += 2) {
	  FastVar var = solp.subst.data[i];
	  FastTerm res = solp.subst.data[i + 1];
	  if (!occurs(var, solp.iterm)) {
	    solp.rhs = applyUnitySubst(solp.rhs, var, res);
	    solp.constraint = applyUnitySubst(solp.constraint, var, res);
	  } else {
	    newsubst.addToSubst(var, res);
	  }
	}
	solp.subst = newsubst;
	next.push_back(solp);
      }
    }
    current = next;
  }
  return result;
}

string toString(const SmtSearchSolution &solution)
{
  ostringstream oss;

  oss << "(" << toString(solution.iterm) << " => " << toString(solution.rhs) << ")";
  oss << toString(solution.subst) << " if " << toString(solution.constraint) << " (used rule " << toString(solution.lhs) << " /\\ " << toString(solution.ruleConstraint) << " => ...).";

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
    
    vector<SmtUnifySolution> topMostSols = smtUnify(cterm, lhs);
    if (topMostSols.size() > 0) {
      LOG(DEBUG5, cerr << "smtSearchRewriteRuleInternal " <<
	  toString(cterm) << " " << toString(iterm) << " " << toString(iconstraint) << " w/ rule " <<
	  toString(lhs) << " " << toString(rhs) << " if " << toString(constraint));
      LOG(DEBUG5, cerr << "smtSearchRewriteRuleInternal has " << topMostSols.size() << " solutions:");
    }
    for (uint i = 0; i < topMostSols.size(); ++i) {
      SmtUnifySolution sol = topMostSols[i];
      LOG(DEBUG5, cerr << "Solution #" << (i + 1) << ": " << toString(sol));
      LOG(DEBUG5, cerr << "Rule is " << toString(lhs) << " => " << toString(rhs) << " if " << toString(constraint));
      LOG(DEBUG5, cerr << "Cterm is " << toString(cterm) << " if " << toString(iconstraint) << "(part of " << toString(iterm) << ")");
      SmtSearchSolution solSearch(iterm, lhs, constraint, sol.subst.applySubst(rhs), sol.subst,
				  fastAnd(fastAnd(
						  sol.subst.applySubst(iconstraint),
						  sol.subst.applySubst(constraint)), sol.constraint));
      solutions.push_back(solSearch);
      LOG(DEBUG5, cerr << "Push search solution #" << (i + 1) << ": " << toString(solSearch)); 
    }

    for (uint i = 0; i < getArity(func); ++i) {
      FastTerm nt = getArg(cterm, i);
      vector<SmtSearchSolution> innerSolutions;
      smtSearchRewriteRuleInternal(nt, iterm, iconstraint, lhs, rhs, constraint, innerSolutions);

      for (uint j = 0; j < innerSolutions.size(); ++j) {
	SmtSearchSolution &sol = innerSolutions[j];
	vector<FastTerm> newArguments;
	for (uint k = 0; k < getArity(func); ++k) {
	  newArguments.push_back(getArg(cterm, k));
	}
	newArguments[i] = sol.rhs;
	sol.rhs = newFuncTerm(func, &newArguments[0]);
      }
      solutions.insert(solutions.end(), innerSolutions.begin(), innerSolutions.end());
    }
  }
}

extern FastFunc funcAnd;
extern bool hasEqFunc[MAXSORTS];
extern FastFunc eqFunc[MAXSORTS];

void extractEqualities(FastTerm constraint, vector<pair<FastVar, FastTerm>> &result, const vector<FastVar> &varsRule)
{
  for (uint i = 0; i < result.size(); ++i) {
    constraint = applyUnitySubst(constraint, result[i].first, result[i].second);
  }
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
	       vector<SmtSearchSolution> &result)
{
  LOG(DEBUG4, cerr << "smtSearchRewriteRule " <<
      toString(cterm) << " " << toString(iterm) << " " << toString(iconstraint) << " w/ rule " <<
      toString(lhs) << " " << toString(rhs) << " if " << toString(constraint));
  /* STEP 1: create fresh instance of rewrite rule */
  vector<FastVar> varsRule;
  vector<FastVar> toRename;
  vector<FastVar> newVarsRule;

  varsOf(lhs, varsRule);
  varsOf(rhs, varsRule);
  varsOf(constraint, varsRule);

  std::sort(varsRule.begin(), varsRule.end());
  auto it = std::unique(varsRule.begin(), varsRule.end());
  varsRule.resize(std::distance(varsRule.begin(), it));

  for (uint i = 0; i < varsRule.size(); ++i) {
    if (occurs(varsRule[i], iterm) || occurs(varsRule[i], iconstraint)) {
      toRename.push_back(varsRule[i]);
    } else {
      newVarsRule.push_back(varsRule[i]);
    }
  }

  FastSubst subst;
  for (uint i = 0; i < toRename.size(); ++i) {
    FastVar var = createFreshVariable(getVarSort(toRename[i]));
    //    freshVars.push_back(var);
    newVarsRule.push_back(var);
    subst.addToSubst(toRename[i], var);
  }
  lhs = subst.applySubst(lhs);
  rhs = subst.applySubst(rhs);
  constraint = subst.applySubst(constraint);

  /* STEP 2: perform narrowing */
  vector<SmtSearchSolution> solutions;
  smtSearchRewriteRuleInternal(cterm, iterm, iconstraint, lhs, rhs, constraint, solutions);

  /* STEP 3: try to simplify constraint */
  LOG(DEBUG4, cerr << "smtSearchRewriteRule has " << solutions.size() << " solutions.");
  for (uint i = 0; i < solutions.size(); ++i) {
    SmtSearchSolution sol = solutions[i];
    LOG(DEBUG4, cerr << "Solution #" << (i + 1) << " (before): " << toString(sol));
    vector<pair<FastVar, FastTerm>> equalities;
    LOG(DEBUG5, cerr << "extractEquality ");
    for (uint j = 0; j < newVarsRule.size(); ++j) {
      LOG(DEBUG5, cerr << " extract var " << getVarName(newVarsRule[j]));
    }
    extractEqualities(sol.constraint, equalities, newVarsRule);

    for (uint j = 0; j < equalities.size(); ++j) {
      sol.iterm = simplify(applyUnitySubst(sol.iterm, equalities[j].first, equalities[j].second));
      sol.rhs = simplify(applyUnitySubst(sol.rhs, equalities[j].first, equalities[j].second));
      sol.subst.applyInRange(equalities[j].first, equalities[j].second);
      sol.constraint = simplify(applyUnitySubst(sol.constraint, equalities[j].first, equalities[j].second));
    }
    solutions[i] = sol;
    LOG(DEBUG4, cerr << "Solution #" << (i + 1) << "  (after): " << toString(sol));
  }

  result.insert(result.end(), solutions.begin(), solutions.end());
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

vector<SmtSearchSolution> prune(const vector<SmtSearchSolution> &sols, Z3_context context) {
  vector<SmtSearchSolution> result;
  for (uint i = 0; i < sols.size(); ++i) {
    if (z3_sat_check(context, sols[i].constraint) != Z3_L_FALSE) {
      result.push_back(sols[i]);
    }
  }
  return result;
}

//Buru note: this one inits context evey time.. intended?
vector<SmtSearchSolution> prune(const vector<SmtSearchSolution> &sols)
{
  return prune(sols, init_z3_context());
}

FastTerm smtDefinedSimplifyTopMost(FastTerm cterm, FastTerm iconstraint,
			       const RewriteSystem &rs, Z3_context context)
{
  LOG(DEBUG5, cerr << "smtDefinedSimplifyTopMost " << toString(cterm) << " " << toString(iconstraint));
  FastTerm result = cterm;
  uint count = 0;

  for (uint ri = 0; ri < rs.size(); ++ri) {
    FastTerm lhs = rs[ri].first.term;
    FastTerm rhs = rs[ri].second;
    FastTerm constraint = rs[ri].first.constraint;

    vector<SmtUnifySolution> topMostSols = smtUnify(cterm, lhs);
    LOG(DEBUG6, cerr << "Rule " << toString(lhs) << " => " << toString(rhs) << " if " << toString(constraint));
    LOG(DEBUG6, cerr << "has " << topMostSols.size() << " potential narrows.");
    for (uint i = 0; i < topMostSols.size(); ++i) {
      SmtUnifySolution sol = topMostSols[i];

      LOG(DEBUG6, cerr << "potential solution = " << toString(sol));
      //      simplifySmtUnifySolution(sol);
      //      LOG(DEBUG6, cerr << "potential solution (after simplify) = " << toString(sol));
      
      FastTerm phi = fastNot(fastImplies(iconstraint, sol.subst.applySubst(fastAnd(sol.constraint, constraint))));
      LOG(DEBUG6, cerr << "Checking whether " << toString(phi) << " is satisfiable.");
      if (z3_sat_check(context, phi) == Z3_L_FALSE) {
	LOG(DEBUG6, cerr << "Unsatisfiable, so do simplify.");
	result = sol.subst.applySubst(rhs);
	count++;
      } else {
	LOG(DEBUG6, cerr << "Satisfiable or unknown, so do not simplify.");
      }
    }
  }
  LOG(DEBUG5, cerr << "has " << count << " narrows.");
  if (count == 1) {
    // exactly one rule fires
    return result;
  } else {
    // either several rules or none apply, therefore no simplification
    return cterm;
  }
}

FastTerm smtDefinedSimplifyInternal(FastTerm cterm, FastTerm iconstraint,
				const RewriteSystem &rs, Z3_context context)
{
  LOG(DEBUG5, cerr << "smtDefinedSimplifyInternal " << toString(cterm) << " " << toString(iconstraint));
  if (isVariable(cterm)) {
    // nothing to simplify here
    return cterm;
  } else {
    assert(isFuncTerm(cterm));
    FastFunc func = getFunc(cterm);
    vector<FastTerm> newArgs;
    bool changed = false;
    for (uint i = 0; i < getArity(func); ++i) {
      FastTerm nt = getArg(cterm, i);
      FastTerm ntsimpl = smtDefinedSimplifyTopMost(nt, iconstraint, rs, context);
      newArgs.push_back(ntsimpl);
      if (!eq_term(ntsimpl, nt)) {
	changed = true;
      }
    }
    FastTerm result = cterm;
    if (changed) {
      result = newFuncTerm(func, &newArgs[0]);
    }
    
    result = smtDefinedSimplifyTopMost(result, iconstraint, rs, context);
    LOG(DEBUG5, cerr << "result is " << toString(result));
    return result;
    // TODO: if not optimally reducing, recheck subterms
  }
}

FastTerm smtDefinedSimplify(FastTerm iterm, FastTerm iconstraint, const RewriteSystem &rsprime)
{
  LOG(DEBUG5, cerr << "smtDefinedSimplify " << toString(iterm) << " " << toString(iconstraint));
  /* create fresh version of rewrite system */
  vector<FastVar> vars;
  varsOf(iterm, vars);
  varsOf(iconstraint, vars);
  std::sort(vars.begin(), vars.end());
  auto it = std::unique(vars.begin(), vars.end());
  vars.resize(std::distance(vars.begin(), it));
  
  RewriteSystem rs = renameAway(rsprime, vars);

  /* replace variables by constants */
  vector<FastTerm> cts;
  FastSubst subst;
  for (uint i = 0; i < vars.size(); ++i) {
    FastFunc funcFresh = newConst((string(getVarName(vars[i])) + "C").c_str(), getVarSort(vars[i]));

    extern bool funcIsBuiltin[MAXFUNCS];
    extern BuiltinFuncType builtinFunc[MAXFUNCS];
    funcIsBuiltin[funcFresh] = true;
    builtinFunc[funcFresh] = bltnFreshConstant;
    
    cts.push_back(newFuncTerm(funcFresh, nullptr));
    subst.addToSubst(vars[i], cts[i]);
    LOG(DEBUG5, cerr << "rename " << toString(vars[i]) << " to " << toString(cts[i]));
  }
  iterm = subst.applySubst(iterm);
  iconstraint = subst.applySubst(iconstraint);
  LOG(DEBUG5, cerr << "smtDefinedSimplify for " << toString(iterm) << " /\\ " << toString(iconstraint));

  /* perform simplification */
  Z3_context context = init_z3_context();
  FastTerm result = smtDefinedSimplifyInternal(iterm, iconstraint, rsprime, context);

  /* replace constants by variables */
  for (int i = 0; i < (int)cts.size(); ++i) {
    result = replaceConstWithVar(result, cts[i], vars[i]);
  }
  return result;
}

FastTerm smtDefinedSimplify(const ConstrainedTerm &ct, const RewriteSystem &rs)
{
  return smtDefinedSimplify(ct.term, ct.constraint, rs);
}

vector<ConstrainedTerm> solutionsToTerms(vector<SmtSearchSolution> &sols) {
  vector<ConstrainedTerm> res;
  for (auto &it : sols)
    res.push_back(ConstrainedTerm(it.subst.applySubst(it.rhs),
      it.subst.applySubst(it.constraint)));
  return res;
}
