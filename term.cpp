#include <cassert>
#include <iostream>
#include "term.h"
#include "log.h"
#include "z3driver.h"
#include "factories.h"
#include "varterm.h"
#include "constrainedterm.h"
#include "substitution.h"

using namespace std;

vector<Variable *> Term::vars() 
{
  if (computedVars) {
    return allVars;
  } else {
    computedVars = true;
    return allVars = computeVars();
  }
}

vector<Variable *> Term::computeUniqueVars()
{
  vector<Variable *> myv = this->vars();
  sort(myv.begin(), myv.end());
  vector<Variable *>::iterator last = unique(myv.begin(), myv.end());
  myv.erase(last, myv.end());
  return myv;
}

vector<Variable *> Term::uniqueVars()
{
  if (computedUniqueVars) {
    return allUniqueVars;
  } else {
    computedUniqueVars = true;
    return allUniqueVars = computeUniqueVars();
  }
}

Term *Term::substitute(Substitution &subst)
{
  map<Term *, Term *> cache;
  return computeSubstitution(subst, cache);
}

bool Term::isNormalized(RewriteSystem &rewriteSystem)
{
  map<Term *, bool> cache;
  return computeIsNormalized(rewriteSystem, cache);
}

Term *Term::normalize(RewriteSystem &rewriteSystem, bool optimallyReducing)
{
  Log(DEBUG9) << "Normalizing " << this->toString() << endl;
  map<Term *, Term *> cache;
  Term *result = computeNormalize(rewriteSystem, cache, optimallyReducing);
  //  Log(DEBUG) << "normalize " << this->toString() << " is " << result->toString() << endl;
  return result;
}

bool Term::isInstanceOf(Term *t, Substitution &s)
{
  map<pair<Term *, Term *>, bool> cache;
  return computeIsInstanceOf(t, s, cache);
}

int Term::dagSize()
{
  map<Term *, int> cache;
  return computeDagSize(cache);
}

Term *Term::rewriteTopMost(RewriteSystem &rewrite, Substitution &how)
{
  for (int i = 0; i < len(rewrite); ++i) {
    pair<Term *, Term *> rewriteRule = rewrite[i];
    Term *result = rewriteTopMost(rewriteRule, how);
    if (this != result) {
      return result;
    }
  }
  return this;
}

Term *Term::rewriteTopMost(pair<Term *, Term *> rewriteRule, Substitution &how)
{
  Term *l = rewriteRule.first;
  Term *r = rewriteRule.second;
    
  Substitution subst;
  if (this->isInstanceOf(l, subst)) {
    how = subst;
    return r->substitute(subst);
  }
  return this;
}

bool unabstractSolution(Substitution abstractingSubstitution, ConstrainedSolution &solution)
{
  Log(DEBUG7) << "unabstractSolution" << endl;
  
  Log(DEBUG7) << "Term = " << solution.term->toString() << endl;
  Log(DEBUG7) << "Constraint = " << solution.constraint->toString() << endl;
  Log(DEBUG7) << "Subst = " << solution.subst.toString() << endl;
  Log(DEBUG7) << "LHS Term = " << solution.lhsTerm->toString() << endl;
  Log(DEBUG7) << "Abstracting substitution = " << abstractingSubstitution.toString() << endl;

  Substitution simplifyingSubst;
  for (Substitution::iterator it = abstractingSubstitution.begin(); it != abstractingSubstitution.end(); ++it) {
    Term *lhsTerm = getVarTerm(it->first)->substitute(solution.subst)->substitute(abstractingSubstitution)->substitute(simplifyingSubst);
    Term *rhsTerm = it->second->substitute(simplifyingSubst);
    Log(DEBUG9) << "Processing constraint " << lhsTerm->toString() << " = " << rhsTerm->toString() << endl;
    if (lhsTerm == rhsTerm) {
      Log(DEBUG9) << "Constraint is trivial, skipping" << endl;
      continue;
    }
    bool simplifiedConstraint = false;
    if (lhsTerm->isVarTerm()) {
      Log(DEBUG7) << "Left-hand side is a variable." << endl;
      Variable *var = lhsTerm->getAsVarTerm()->variable;
      Log(DEBUG7) << "Variable " << var->name << " in domain of simplifyingSubst: " << simplifyingSubst.inDomain(var) << endl;
      Log(DEBUG7) << "Term " << rhsTerm->toString() << " has variable " << var->name << ": " << rhsTerm->hasVariable(var) << endl;
      Log(DEBUG7) << "Substitution " << abstractingSubstitution.toString() << " has variable " << var->name << " in range: " << abstractingSubstitution.inRange(var) << endl;
      if ((!(simplifyingSubst.inDomain(var)))) {
	if ((!(rhsTerm->hasVariable(var)))) {
	  if ((!(abstractingSubstitution.inRange(var)))) {
	    Log(DEBUG7) << "Not yet in domain of simplifyingSubst, adding " << var->name << " |-> " << rhsTerm->toString() << "." << endl;
	    simplifiedConstraint = true;
	    simplifyingSubst.add(var, rhsTerm);
	  }
	}
      }
    }
    if (!simplifiedConstraint && rhsTerm->isVarTerm()) {
      Log(DEBUG7) << "Right-hand side is a variable." << endl;
      Variable *var = rhsTerm->getAsVarTerm()->variable;
      Log(DEBUG7) << "Variable " << var->name << " in domain of simplifyingSubst: " << simplifyingSubst.inDomain(var) << endl;
      Log(DEBUG7) << "Term " << lhsTerm->toString() << " has variable " << var->name << ": " << lhsTerm->hasVariable(var) << endl;
      Log(DEBUG7) << "Substitution " << abstractingSubstitution.toString() << " has variable " << var->name << " in range: " << abstractingSubstitution.inRange(var) << endl;
      if (!simplifyingSubst.inDomain(var) && !lhsTerm->hasVariable(var) && !abstractingSubstitution.inRange(var)) {
	Log(DEBUG7) << "Not yet in domain of simplifyingSubst, adding " << var->name << " |-> " << lhsTerm->toString() << "." << endl;
	simplifiedConstraint = true;
	simplifyingSubst.add(var, lhsTerm);
      }
    }
    if (!simplifiedConstraint) {
      Log(DEBUG7) << "Could not simplify constraint, sending to SMT solver." << endl;
      solution.constraint = bAnd(solution.constraint, createEqualityConstraint(lhsTerm, rhsTerm));
    }
  }

  Substitution resultSubst = solution.subst;
  for (Substitution::iterator it = abstractingSubstitution.begin(); it != abstractingSubstitution.end(); ++it) {
    resultSubst.force(it->first, it->second);
  }
  solution.subst = resultSubst;
  solution.simplifyingSubst = simplifyingSubst;

  Log(DEBUG7) << "Solution.subst = " << solution.subst.toString() << endl;
  Log(DEBUG7) << "Solution.simplifyingSubst = " << solution.simplifyingSubst.toString() << endl;

  Log(DEBUG7) << "Checking satisfiability of " << solution.constraint->substitute(solution.subst)->substitute(solution.simplifyingSubst)->toString() << "." << endl;
  Z3Theory theory;
  vector<Variable *> interpretedVariables = getInterpretedVariables();
  for (int i = 0; i < (int)interpretedVariables.size(); ++i) {
    theory.addVariable(interpretedVariables[i]);
  }
  theory.addConstraint(solution.constraint->substitute(solution.subst)->substitute(solution.simplifyingSubst));
  if (theory.isSatisfiable() != unsat) {
    Log(DEBUG7) << "Possibly satisfiable." << endl;
    return true;
  }
  Log(DEBUG7) << "Surely unsatisfiable." << endl;
  return false;
}

bool Term::unifyModuloTheories(Term *other, Substitution &resultSubstitution, Term *&resultConstraint)
{
  Log(DEBUG6) << "unifyModuloTheories " << this->toString() << " and " <<
    other->toString() << endl;
  Substitution abstractingSubstitution;

  Term *abstractTerm = this->abstract(abstractingSubstitution);
  Log(DEBUG6) << "Abstract term: " << abstractTerm->toString() << endl;
  Log(DEBUG6) << "Abstracting substitution: " << abstractingSubstitution.toString() << endl;
  
  Substitution unifyingSubstitution;
  if (abstractTerm->unifyWith(other, unifyingSubstitution)) {
    Log(DEBUG6) << "Syntactic unification succeeded. Unifying substitution: " << endl;
    Log(DEBUG6) << unifyingSubstitution.toString() << endl;
    Term *whatever = bTrue();
    ConstrainedSolution sol(whatever, bTrue(), unifyingSubstitution, whatever);

    if (unabstractSolution(abstractingSubstitution, sol)) {
      resultSubstitution = sol.subst; // TODO: compus cu simplifyingSubst?;
      resultConstraint = sol.constraint->substitute(sol.simplifyingSubst);
      for (Substitution::iterator it = sol.simplifyingSubst.begin(); it != sol.simplifyingSubst.end(); ++it) {
	      resultSubstitution.force(it->first, it->second);
      }
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

vector<ConstrainedSolution> Term::smtNarrowSearchBasic(RewriteSystem &rsInit, Term *initialConstraint)
{
  vector<ConstrainedSolution> finalResult;

  Substitution abstractingSubstitution;

  // STEP 1: compute abstracted term (and constraining substitution)
  Log(DEBUG5) << "Term::smtNarrowSearchBasic(ConstrainedRewriteSystem &, Term *) " <<
    this->toString() << " /\\ " << initialConstraint->toString() << endl;

  Term *abstractTerm = this->abstract(abstractingSubstitution);

  Log(DEBUG6) << "Abstract term: " << abstractTerm->toString() << endl;
  Log(DEBUG6) << "Abstracting substitution: " << abstractingSubstitution.toString() << endl;

  // STEP 2: perform one-step narrowing from the abstract term
  Log(DEBUG7) << "Rewrite system: " << rsInit.toString() << endl;
  RewriteSystem rs = rsInit.fresh();
  Log(DEBUG7) << "Fresh rewrite system: " << rs.toString() << endl;
  vector<ConstrainedSolution> solutions = abstractTerm->narrowSearch(rs);

  Log(DEBUG6) << "Narrowing abstract term resulted in " << solutions.size() << " solutions" << endl;
  
  if (initialConstraint == 0) {
    initialConstraint = bTrue();
  }

  // STEP 3.2: check that the constraints are satisfiable
  for (int i = 0; i < (int)solutions.size(); ++i) {
    ConstrainedSolution sol = solutions[i];
    sol.constraint = bAnd(initialConstraint, sol.constraint);

    if (unabstractSolution(abstractingSubstitution, sol)) {
      finalResult.push_back(sol);
    }
  }

  return finalResult;
}

vector<ConstrainedSolution> Term::smtNarrowSearchBasic(ConstrainedRewriteSystem &crsInit, Term *initialConstraint)
{
  vector<ConstrainedSolution> finalResult;

  Substitution abstractingSubstitution;

  // STEP 1: compute abstracted term (and constraining substitution)
  Log(DEBUG) << "Term::smtNarrowSearchBasic(ConstrainedRewriteSystem &, Term *) " <<
    this->toString() << " /\\ " << initialConstraint->toString() << endl;

  Term *abstractTerm = this->abstract(abstractingSubstitution);

  Log(DEBUG) << "Abstract term: " << abstractTerm->toString() << endl;
  Log(DEBUG) << "Abstracting substitution: " << abstractingSubstitution.toString() << endl;

  // STEP 2: perform one-step narrowing from the abstract term
  Log(DEBUG9) << "Conditional system: " << crsInit.toString() << endl;
  ConstrainedRewriteSystem crs = crsInit.fresh(abstractTerm->vars());
  Log(DEBUG9) << "Fresh rewrite system: " << crs.toString() << endl;
  vector<ConstrainedSolution> solutions = abstractTerm->narrowSearch(crs);

  Log(DEBUG) << "Narrowing abstract term resulted in " << solutions.size() << " solutions" << endl;
  
  // STEP 3: check that the narrowing constraints are satisfiable
  if (initialConstraint == 0) {
    initialConstraint = bTrue();
  }

  // STEP 3.2: check that the constraints are satisfiable
  for (int i = 0; i < (int)solutions.size(); ++i) {
    ConstrainedSolution sol = solutions[i];

    sol.constraint = bAnd(initialConstraint, sol.constraint);

    if (unabstractSolution(abstractingSubstitution, sol)) {
      finalResult.push_back(sol);
    }
  }

  return finalResult;
}

vector<ConstrainedSolution> Term::smtNarrowSearchWdf(RewriteSystem &rsInit, Term *initialConstraint)
{
  Log(DEBUG7) << "Term::smtNarrowSearchWdt" << this->toString() << endl;
  Term *searchStart = this;
  if (hasDefinedFunctions) {
    RewriteSystem functionsRS = getRewriteSystem("functions");
    searchStart = this->normalize(functionsRS, false);
  }
  return searchStart->smtNarrowSearchBasic(rsInit, initialConstraint);
}

vector<ConstrainedSolution> Term::smtNarrowSearchWdf(ConstrainedRewriteSystem &crsInit, Term *initialConstraint)
{
  Log(DEBUG7) << "Term::smtNarrowSearchWdt" << this->toString() << endl;
  Term *searchStart = this;
  if (hasDefinedFunctions) {
    RewriteSystem functionsRS = getRewriteSystem("functions");
    searchStart = this->normalize(functionsRS, false);
  }
  return searchStart->smtNarrowSearchBasic(crsInit, initialConstraint);
}

bool Term::hasVariable(Variable *var)
{
  vector<Variable *> uvars = uniqueVars();
  for (vector<Variable *>::iterator it = uvars.begin(); it != uvars.end(); ++it) {
    if (*it == var) {
      return true;
    }
  }
  return false;
}
