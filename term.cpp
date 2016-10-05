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
  Log(DEBUG5) << "Term::smtNarrowSearch(RewriteSystem &, Term *) " << this->toString() << " /\\ " << initialConstraint->toString() << endl;
  Term *abstractTerm = this->abstract(substitution);

  Log(DEBUG6) << "Abstract term: " << abstractTerm->toString() << endl;
  Log(DEBUG6) << "Abstracting substitution: " << substitution.toString() << endl;

  // STEP 2: perform one-step narrowing from the abstract term
  //  Log(DEBUG6) << "Rewrite system: " << rsInit.toString() << endl;
  RewriteSystem rs = rsInit.fresh(abstractTerm->vars());
  //  Log(DEBUG6) << "Fresh rewrite system: " << rs.toString() << endl;
  vector<Solution> solutions = abstractTerm->narrowSearch(rs);

  Log(DEBUG6) << "Narrowing abstract term resulted in " << solutions.size() << " solutions" << endl;

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

  if (initialConstraint == 0) {
    initialConstraint = bTrue();
  }

  // STEP 3.2: check that the constraints are satisfiable
  for (int i = 0; i < solutions.size(); ++i) {
    Log(DEBUG6) << "Solution " << i << ": " << solutions[i].term->toString() << " " << solutions[i].substitution.toString() << endl;

    Solution sol = solutions[i];
    ConstrainedTerm constrainedTerm(sol.term->substitute(sol.substitution), initialConstraint);

    // STEP 3.2.1: start from the generic theory
    Z3Theory solTheory(theory); 

    // STEP 3.2.2: add constraints
    Substitution resultSubstitution;

    Term *lhsTrouble, *rhsTrouble;
    solTheory.addConstraint(initialConstraint);
    for (Substitution::iterator it = substitution.begin(); it != substitution.end(); ++it) {
      Term *lhsTerm = getVarTerm(it->first)->substitute(sol.substitution);
      Term *rhsTerm = it->second->substitute(sol.substitution);
      if (it->second != it->second->substitute(sol.substitution)) {
	Log(ERROR) << "Trouble ahead" << endl;
	assert(0);
      }
      solTheory.addEqualityConstraint(lhsTerm, rhsTerm);
      if (lhsTerm != rhsTerm) {
  	if (lhsTerm->isVariable()) {
  	  Variable *var = ((VarTerm *)lhsTerm)->variable;
  	  resultSubstitution.force(var, rhsTerm);

	  constrainedTerm.constraint = bAnd(constrainedTerm.constraint, createEqualityConstraint(lhsTerm, rhsTerm));
  	} else if (rhsTerm->isVariable()) {
  	  Variable *var = ((VarTerm *)rhsTerm)->variable;
  	  resultSubstitution.force(var, lhsTerm);

	  constrainedTerm.constraint = bAnd(constrainedTerm.constraint, createEqualityConstraint(lhsTerm, rhsTerm));
  	} else {
	  constrainedTerm.constraint = bAnd(constrainedTerm.constraint, createEqualityConstraint(lhsTerm, rhsTerm));
  	}
      }
      //    fprintf(stderr, "Declaring constraint %s = %s.\n", getVarTerm(it->first)->toString().c_str(),
      //	    it->second->toString().c_str());
    }

    // STEP 3.2.3: call z3 to check satisfiability
    if (solTheory.isSatisfiable() != unsat) {
      // there's a chance the constraints are satisfiable

      constrainedTerm.term = constrainedTerm.term->substitute(resultSubstitution);
      constrainedTerm.constraint = simplifyConstraint(constrainedTerm.constraint->substitute(resultSubstitution));

      Log(DEBUG5) << "Introducing exists on " << constrainedTerm.term->toString() << endl;
      Log(DEBUG5) << "                   /\\ " << constrainedTerm.constraint->toString() << endl;

      vector<Variable *> varsMore = constrainedTerm.constraint->vars();
      vector<Variable *> varsLess = this->vars(); // constrainedTerm.term->vars();
      vector<Variable *> vars;
      for (vector<Variable *>::iterator it = varsMore.begin(); it != varsMore.end(); ++it) {
	bool found = false;
	for (vector<Variable *>::iterator jt = varsLess.begin(); jt != varsLess.end(); ++jt) {
	  Variable *my = *it;
	  Variable *te = *jt;
	  if (my == te) {
	    found = true;
	    break;
	  }
	}
	for (vector<Variable *>::iterator jt = vars.begin(); jt != vars.end(); ++jt) {
	  Variable *my = *it;
	  Variable *te = *jt;
	  if (my == te) {
	    found = true;
	    break;
	  }
	}
	if (!found) {
	  vars.push_back(*it);
	}
      }
      for (vector<Variable *>::iterator it = vars.begin(); it != vars.end(); ++it) {
	constrainedTerm.constraint = bExists(*it, constrainedTerm.constraint);
      }

      Log(DEBUG4) << "Resulting constrainedTerm = " << constrainedTerm.toString() << endl;

      finalResult.push_back(constrainedTerm);
    }
  }

  return finalResult;
}

vector<ConstrainedTerm> Term::smtNarrowSearch(CRewriteSystem &crsInit, Term *initialConstraint)
{
  vector<ConstrainedTerm> finalResult;

  Substitution substitution;

  // STEP 1: compute abstracted term (and constraining substitution)
  Log(DEBUG5) << "Term::smtNarrowSearch(CRewriteSystem &, Term *) " << this->toString() << " /\\ " << initialConstraint->toString() << endl;

  Term *abstractTerm = this->abstract(substitution);

  Log(DEBUG6) << "Abstract term: " << abstractTerm->toString() << endl;
  Log(DEBUG6) << "Abstracting substitution: " << substitution.toString() << endl;

  // STEP 2: perform one-step narrowing from the abstract term
  Log(DEBUG6) << "Conditional system: " << crsInit.toString() << endl;
  CRewriteSystem crs = crsInit.fresh(abstractTerm->vars());
  Log(DEBUG6) << "Fresh rewrite system: " << crs.toString() << endl;
  vector<ConstrainedSolution> solutions = abstractTerm->narrowSearch(crs);

  Log(DEBUG6) << "Narrowing abstract term resulted in " << solutions.size() << " solutions" << endl;
  
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

  if (initialConstraint == 0) {
    initialConstraint = bTrue();
  }

  // STEP 3.2: check that the constraints are satisfiable
  for (int i = 0; i < solutions.size(); ++i) {
    ConstrainedSolution sol = solutions[i];
    ConstrainedTerm constrainedTerm(sol.term, bAnd(initialConstraint, sol.constraint));

    // STEP 3.2.1: start from the generic theory
    Z3Theory solTheory(theory); 

    // STEP 3.2.2: add constraints
    Substitution resultSubstitution;

    Term *lhsTrouble, *rhsTrouble;
    solTheory.addConstraint(initialConstraint);
    for (Substitution::iterator it = substitution.begin(); it != substitution.end(); ++it) {
      Term *lhsTerm = getVarTerm(it->first)->substitute(sol.substitution);
      Term *rhsTerm = it->second;
      if (it->second != it->second->substitute(sol.substitution)) {
	Log(ERROR) << "TROUBLE AHEAD" << endl;
	assert(0);
      }
      solTheory.addEqualityConstraint(lhsTerm, rhsTerm);
      if (lhsTerm != rhsTerm) {
  	if (lhsTerm->isVariable()) {
  	  Variable *var = ((VarTerm *)lhsTerm)->variable;
  	  resultSubstitution.force(var, rhsTerm);

	  constrainedTerm.constraint = bAnd(constrainedTerm.constraint, createEqualityConstraint(lhsTerm, rhsTerm));
  	} else if (rhsTerm->isVariable()){
  	  Variable *var = ((VarTerm *)rhsTerm)->variable;
  	  resultSubstitution.force(var, lhsTerm);

	  constrainedTerm.constraint = bAnd(constrainedTerm.constraint, createEqualityConstraint(lhsTerm, rhsTerm));
  	} else {
	  constrainedTerm.constraint = bAnd(constrainedTerm.constraint, createEqualityConstraint(lhsTerm, rhsTerm));
  	}
      }
    }

    // STEP 3.2.3: call z3 to check satisfiability
    if (solTheory.isSatisfiable() != unsat) {
      // there's a chance the constraints are satisfiable
      // there's a chance the constraints are satisfiable

      constrainedTerm.term = constrainedTerm.term->substitute(resultSubstitution);
      constrainedTerm.constraint = simplifyConstraint(constrainedTerm.constraint->substitute(resultSubstitution));

      Log(DEBUG5) << "Introducing exists on " << constrainedTerm.term->toString() << endl;
      Log(DEBUG5) << "                   /\\ " << constrainedTerm.constraint->toString() << endl;

      vector<Variable *> varsMore = constrainedTerm.constraint->vars();
      vector<Variable *> varsLess = this->vars(); // constrainedTerm.term->vars();
      vector<Variable *> vars;
      for (vector<Variable *>::iterator it = varsMore.begin(); it != varsMore.end(); ++it) {
	bool found = false;
	for (vector<Variable *>::iterator jt = varsLess.begin(); jt != varsLess.end(); ++jt) {
	  Variable *my = *it;
	  Variable *te = *jt;
	  if (my == te) {
	    found = true;
	    break;
	  }
	}
	for (vector<Variable *>::iterator jt = vars.begin(); jt != vars.end(); ++jt) {
	  Variable *my = *it;
	  Variable *te = *jt;
	  if (my == te) {
	    found = true;
	    break;
	  }
	}
	if (!found) {
	  vars.push_back(*it);
	}
      }
      for (vector<Variable *>::iterator it = vars.begin(); it != vars.end(); ++it) {
	constrainedTerm.constraint = bExists(*it, constrainedTerm.constraint);
      }

      finalResult.push_back(constrainedTerm);
    }
  }

  return finalResult;
}
