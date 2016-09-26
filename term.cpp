#include <cassert>
#include <iostream>
#include "term.h"
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
    //  fprintf(stderr, "Preparing z3 theory.\n");
    vector<Variable *> interpretedVariables = getInterpretedVariables();
    for (int i = 0; i < interpretedVariables.size(); ++i) {
      theory.addVariable(interpretedVariables[i]);
      //    fprintf(stderr, "Declaring variable %s : %s.\n", interpretedVariables[i]->name.c_str(),
      //	    interpretedVariables[i]->sort->name.c_str());
    }
    Function *TrueFun = getFunction("true");
    Function *AndFun = getFunction("band");
    Function *EqualsFun = getFunction("bequals");
    if (initialConstraint == 0) {
      vector<Term *> empty;
      initialConstraint = getFunTerm(TrueFun, empty);
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
	  vector<Term *> arguments;
	  arguments.push_back(resultConstraint);
	  vector<Term *> sides;
	  sides.push_back(lhsTerm);
	  sides.push_back(rhsTerm);
	  arguments.push_back(getFunTerm(EqualsFun, sides));
	  resultConstraint = getFunTerm(AndFun, arguments);
  	} else if (rhsTerm->isVariable()){
	  Variable *var = ((VarTerm *)rhsTerm)->variable;
	  resultSubstitution.force(var, lhsTerm);
	  vector<Term *> arguments;
	  arguments.push_back(resultConstraint);
	  vector<Term *> sides;
	  sides.push_back(lhsTerm);
	  sides.push_back(rhsTerm);
	  arguments.push_back(getFunTerm(EqualsFun, sides));
	  resultConstraint = getFunTerm(AndFun, arguments);
  	} else {
	  vector<Term *> arguments;
	  arguments.push_back(resultConstraint);
	  vector<Term *> sides;
	  sides.push_back(lhsTerm);
	  sides.push_back(rhsTerm);
	  arguments.push_back(getFunTerm(EqualsFun, sides));
	  resultConstraint = getFunTerm(AndFun, arguments);
  	}
      }
      //    fprintf(stderr, "Declaring constraint %s = %s.\n", getVarTerm(it->first)->toString().c_str(),
      //	    it->second->toString().c_str());
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

vector<ConstrainedTerm> Term::smtNarrowSearch(RewriteSystem &rsInit, Term *initialConstraint)
{
  vector<ConstrainedTerm> finalResult;

  Substitution substitution;
  // STEP 1: compute abstracted term (and constraining substitution)
  Term *abstractTerm = this->abstract(substitution);

  //  cerr << "Abstracted term: " << abstractTerm->toString() << endl;

  //  cerr << "Rewrite system has " << rs.size() << " rules." << endl;
  // STEP 2: perform one-step narrowing from the abstract term
  RewriteSystem rs = rsInit.fresh(abstractTerm->vars());
  vector<Solution> solutions = abstractTerm->narrowSearch(rs);

  //  cerr << "Narrowing results in " << solutions.size() << " solutions." << endl;
  
  // STEP 3: check that the narrowing constraints are satisfiable
  // STEP 3.1: prepare generic theory for Z3
  Z3Theory theory;
  //  fprintf(stderr, "Preparing z3 theory.\n");
  vector<Variable *> interpretedVariables = getInterpretedVariables();
  for (int i = 0; i < interpretedVariables.size(); ++i) {
    theory.addVariable(interpretedVariables[i]);
    //    fprintf(stderr, "Declaring variable %s : %s.\n", interpretedVariables[i]->name.c_str(),
    //	    interpretedVariables[i]->sort->name.c_str());
  }

  Function *TrueFun = getFunction("true");
  Function *AndFun = getFunction("band");
  Function *EqualsFun = getFunction("bequals");
  if (initialConstraint == 0) {
    vector<Term *> empty;
    initialConstraint = getFunTerm(TrueFun, empty);
  }

  // STEP 3.2: check that the constraints are satisfiable
  for (int i = 0; i < solutions.size(); ++i) {
    Solution sol = solutions[i];
    ConstrainedTerm constrainedTerm;

    constrainedTerm.term = sol.term;
    constrainedTerm.constraint = initialConstraint;

    // STEP 3.2.1: start from the generic theory
    Z3Theory solTheory(theory); 

    // STEP 3.2.2: add constraints
    Substitution resultSubstitution;

    Term *lhsTrouble, *rhsTrouble;
    solTheory.addConstraint(initialConstraint);
    for (Substitution::iterator it = substitution.begin(); it != substitution.end(); ++it) {
      Term *lhsTerm = getVarTerm(it->first)->substitute(sol.substitution);
      Term *rhsTerm = it->second->substitute(sol.substitution);
      solTheory.addEqualityConstraint(lhsTerm, rhsTerm);
      if (lhsTerm != rhsTerm) {
  	if (lhsTerm->isVariable()) {
  	  Variable *var = ((VarTerm *)lhsTerm)->variable;
  	  resultSubstitution.force(var, rhsTerm);
	  vector<Term *> arguments;
	  arguments.push_back(constrainedTerm.constraint);
	  vector<Term *> sides;
	  sides.push_back(lhsTerm);
	  sides.push_back(rhsTerm);
	  arguments.push_back(getFunTerm(EqualsFun, sides));
	  constrainedTerm.constraint = getFunTerm(AndFun, arguments);
  	} else if (rhsTerm->isVariable()){
  	  Variable *var = ((VarTerm *)rhsTerm)->variable;
  	  resultSubstitution.force(var, lhsTerm);
	  vector<Term *> arguments;
	  arguments.push_back(constrainedTerm.constraint);
	  vector<Term *> sides;
	  sides.push_back(lhsTerm);
	  sides.push_back(rhsTerm);
	  arguments.push_back(getFunTerm(EqualsFun, sides));
	  constrainedTerm.constraint = getFunTerm(AndFun, arguments);
  	} else {
	  vector<Term *> arguments;
	  arguments.push_back(constrainedTerm.constraint);
	  vector<Term *> sides;
	  sides.push_back(lhsTerm);
	  sides.push_back(rhsTerm);
	  arguments.push_back(getFunTerm(EqualsFun, sides));
	  constrainedTerm.constraint = getFunTerm(AndFun, arguments);
  	}
      }
      //    fprintf(stderr, "Declaring constraint %s = %s.\n", getVarTerm(it->first)->toString().c_str(),
      //	    it->second->toString().c_str());
    }

    // STEP 3.2.3: call z3 to check satisfiability
    if (solTheory.isSatisfiable() != unsat) {
      // there's a chance the constraints are satisfiable

      constrainedTerm.term = constrainedTerm.term->substitute(resultSubstitution);

      finalResult.push_back(constrainedTerm);
    }
  }

  return finalResult;
}
