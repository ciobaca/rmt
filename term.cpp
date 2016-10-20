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

Term *Term::normalize(RewriteSystem &rewriteSystem)
{
  map<Term *, Term *> cache;
  return computeNormalize(rewriteSystem, cache);
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

bool Term::smtUnifyWith(Term *other, Term *initialConstraint,
		   Substitution &resultSubstitution, Term *&resultConstraint)
{
  Substitution abstractingSubstitution;
  // STEP 1: compute abstracted term (and constraining substitution)
  Term *abstractTerm = this->abstract(abstractingSubstitution);

  Substitution unifyingSubstitution;
  if (abstractTerm->unifyWith(other, unifyingSubstitution)) {
    Z3Theory theory;
    vector<Variable *> interpretedVariables = getInterpretedVariables();
    for (int i = 0; i < interpretedVariables.size(); ++i) {
      theory.addVariable(interpretedVariables[i]);
    }

    if (initialConstraint == 0) {
      initialConstraint = bTrue();
    }

    Term *lhsTrouble, *rhsTrouble;
    theory.addConstraint(initialConstraint);
    resultConstraint = initialConstraint;
    for (Substitution::iterator it = abstractingSubstitution.begin();
	 it != abstractingSubstitution.end(); ++it) {
      Term *lhsTerm = getVarTerm(it->first)->substitute(unifyingSubstitution);
      Term *rhsTerm = it->second->substitute(unifyingSubstitution);
      theory.addEqualityConstraint(lhsTerm, rhsTerm);
      if (lhsTerm != rhsTerm) {
  	if (lhsTerm->isVariable()) {
  	  Variable *var = ((VarTerm *)lhsTerm)->variable;
  	  resultSubstitution.force(var, rhsTerm);

	  resultConstraint = bAnd(resultConstraint, createEqualityConstraint(lhsTerm, rhsTerm));
  	} else if (rhsTerm->isVariable()){
	  Variable *var = ((VarTerm *)rhsTerm)->variable;
	  resultSubstitution.force(var, lhsTerm);

	  resultConstraint = bAnd(resultConstraint, createEqualityConstraint(lhsTerm, rhsTerm));
  	} else {
	  resultConstraint = bAnd(resultConstraint, createEqualityConstraint(lhsTerm, rhsTerm));
  	}
      }
    }

    // STEP 3.2.3: call z3 to check satisfiability
    if (theory.isSatisfiable() != unsat) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

vector<ConstrainedSolution> Term::smtNarrowSearch(RewriteSystem &rsInit, Term *initialConstraint)
{
  vector<ConstrainedSolution> finalResult;

  Substitution abstractingSubstitution;

  // STEP 1: compute abstracted term (and constraining substitution)
  Log(DEBUG5) << "Term::smtNarrowSearch(CRewriteSystem &, Term *) " <<
    this->toString() << " /\\ " << initialConstraint->toString() << endl;

  Term *abstractTerm = this->abstract(abstractingSubstitution);

  Log(DEBUG6) << "Abstract term: " << abstractTerm->toString() << endl;
  Log(DEBUG6) << "Abstracting substitution: " << abstractingSubstitution.toString() << endl;

  // STEP 2: perform one-step narrowing from the abstract term
  Log(DEBUG7) << "Rewrite system: " << rsInit.toString() << endl;
  RewriteSystem rs = rsInit.fresh(abstractTerm->vars());
  Log(DEBUG7) << "Fresh rewrite system: " << rs.toString() << endl;
  vector<ConstrainedSolution> solutions = abstractTerm->narrowSearch(rs);

  Log(DEBUG6) << "Narrowing abstract term resulted in " << solutions.size() << " solutions" << endl;
  
  // STEP 3: check that the narrowing constraints are satisfiable
  // STEP 3.1: prepare generic theory for Z3
  Z3Theory theory;
  vector<Variable *> interpretedVariables = getInterpretedVariables();
  for (int i = 0; i < interpretedVariables.size(); ++i) {
    theory.addVariable(interpretedVariables[i]);
  }

  if (initialConstraint == 0) {
    initialConstraint = bTrue();
  }

  // STEP 3.2: check that the constraints are satisfiable
  for (int i = 0; i < solutions.size(); ++i) {
    ConstrainedSolution sol = solutions[i];
    sol.term = sol.term->substitute(abstractingSubstitution);
    sol.constraint = sol.constraint->substitute(abstractingSubstitution);

    Log(DEBUG7) << "Solution #" << i << endl;
    Log(DEBUG7) << "Term = " << sol.term->toString() << endl;
    Log(DEBUG7) << "Constraint = " << sol.constraint->toString() << endl;
    Log(DEBUG7) << "Subst = " << sol.subst.toString() << endl;
    Log(DEBUG7) << "LHS Term = " << sol.lhsTerm->toString() << endl;

    sol.constraint = bAnd(initialConstraint, sol.constraint);

    // STEP 3.2.1: start from the generic theory

    Substitution simplifyingSubst;
    // STEP 3.2.2: add constraints resulting from abstracting theory
    for (Substitution::iterator it = abstractingSubstitution.begin(); it != abstractingSubstitution.end(); ++it) {
      Term *lhsTerm = getVarTerm(it->first)->substitute(sol.subst)->substitute(abstractingSubstitution);
      Term *rhsTerm = it->second;
      if (lhsTerm == rhsTerm) {
	continue;
      }
      bool simplifiedConstraint = false;
      if (lhsTerm->isVariable()) {
	Variable *var = ((VarTerm *)lhsTerm)->variable;
	if (!simplifyingSubst.inDomain(var)) {
	  simplifiedConstraint = true;
	  simplifyingSubst.add(var, rhsTerm);
	}
      }
      if (!simplifiedConstraint && rhsTerm->isVariable()) {
	Variable *var = ((VarTerm *)rhsTerm)->variable;
	if (!simplifyingSubst.inDomain(var)) {
	  simplifiedConstraint = true;
	  simplifyingSubst.add(var, lhsTerm);
	}
      }
      if (!simplifiedConstraint) {
	sol.constraint = bAnd(sol.constraint, createEqualityConstraint(lhsTerm, rhsTerm));
      }
    }
    sol.constraint = sol.constraint->substitute(simplifyingSubst);
    sol.term = sol.term->substitute(simplifyingSubst);

    Z3Theory solTheory(theory); 
    solTheory.addConstraint(sol.constraint);
    // STEP 3.2.3: call z3 to check satisfiability
    if (solTheory.isSatisfiable() != unsat) {
      finalResult.push_back(sol);
    }
  }

  return finalResult;
}

vector<ConstrainedSolution> Term::smtNarrowSearch(CRewriteSystem &crsInit, Term *initialConstraint)
{
  vector<ConstrainedSolution> finalResult;

  Substitution abstractingSubstitution;

  // STEP 1: compute abstracted term (and constraining substitution)
  Log(DEBUG5) << "Term::smtNarrowSearch(CRewriteSystem &, Term *) " <<
    this->toString() << " /\\ " << initialConstraint->toString() << endl;

  Term *abstractTerm = this->abstract(abstractingSubstitution);

  Log(DEBUG6) << "Abstract term: " << abstractTerm->toString() << endl;
  Log(DEBUG6) << "Abstracting substitution: " << abstractingSubstitution.toString() << endl;

  // STEP 2: perform one-step narrowing from the abstract term
  Log(DEBUG7) << "Conditional system: " << crsInit.toString() << endl;
  CRewriteSystem crs = crsInit.fresh(abstractTerm->vars());
  Log(DEBUG7) << "Fresh rewrite system: " << crs.toString() << endl;
  vector<ConstrainedSolution> solutions = abstractTerm->narrowSearch(crs);

  Log(DEBUG6) << "Narrowing abstract term resulted in " << solutions.size() << " solutions" << endl;
  
  // STEP 3: check that the narrowing constraints are satisfiable
  // STEP 3.1: prepare generic theory for Z3
  Z3Theory theory;
  vector<Variable *> interpretedVariables = getInterpretedVariables();
  for (int i = 0; i < interpretedVariables.size(); ++i) {
    theory.addVariable(interpretedVariables[i]);
  }

  if (initialConstraint == 0) {
    initialConstraint = bTrue();
  }

  // STEP 3.2: check that the constraints are satisfiable
  for (int i = 0; i < solutions.size(); ++i) {
    ConstrainedSolution sol = solutions[i];
    sol.term = sol.term->substitute(abstractingSubstitution);
    sol.constraint = sol.constraint->substitute(abstractingSubstitution);

    Log(DEBUG7) << "Solution #" << i << endl;
    Log(DEBUG7) << "Term = " << sol.term->toString() << endl;
    Log(DEBUG7) << "Constraint = " << sol.constraint->toString() << endl;
    Log(DEBUG7) << "LHS Term = " << sol.lhsTerm->toString() << endl;

    sol.constraint = bAnd(initialConstraint, sol.constraint);

    // STEP 3.2.1: start from the generic theory

    Substitution simplifyingSubst;
    // STEP 3.2.2: add constraints resulting from abstracting theory
    for (Substitution::iterator it = abstractingSubstitution.begin(); it != abstractingSubstitution.end(); ++it) {
      Term *lhsTerm = getVarTerm(it->first)->substitute(sol.subst)->substitute(abstractingSubstitution);
      Term *rhsTerm = it->second;
      if (lhsTerm == rhsTerm) {
	continue;
      }
      bool simplifiedConstraint = false;
      if (lhsTerm->isVariable()) {
	Variable *var = ((VarTerm *)lhsTerm)->variable;
	if (!simplifyingSubst.inDomain(var)) {
	  simplifiedConstraint = true;
	  simplifyingSubst.add(var, rhsTerm);
	}
      }
      if (!simplifiedConstraint && rhsTerm->isVariable()) {
	Variable *var = ((VarTerm *)rhsTerm)->variable;
	if (!simplifyingSubst.inDomain(var)) {
	  simplifiedConstraint = true;
	  simplifyingSubst.add(var, lhsTerm);
	}
      }
      if (!simplifiedConstraint) {
	sol.constraint = bAnd(sol.constraint, createEqualityConstraint(lhsTerm, rhsTerm));
      }
    }
    sol.constraint = sol.constraint->substitute(simplifyingSubst);
    sol.term = sol.term->substitute(simplifyingSubst);

    Z3Theory solTheory(theory); 
    solTheory.addConstraint(sol.constraint);
    // STEP 3.2.3: call z3 to check satisfiability
    if (solTheory.isSatisfiable() != unsat) {
      finalResult.push_back(sol);
    }
  }

  return finalResult;
}
