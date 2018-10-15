#include "funterm.h"
#include "varterm.h"
#include "log.h"
#include "factories.h"
#include "helper.h"
#include "sort.h"
#include "z3driver.h"
#include <sstream>
#include <cassert>
#include <iostream>

using namespace std;

FunTerm::FunTerm(Function *function, vector<Term *> arguments) :
  function(function),
  arguments(arguments)
{
  hasDefinedFunctions = function->isDefined;
  assert(this->arguments->size() == function->arguments.size());
  if (!hasDefinedFunctions) {
    for (vector<Term *>::iterator it = arguments.begin(); it != arguments.end(); ++it) {
      if ((*it)->hasDefinedFunctions) {
        hasDefinedFunctions = true;
        break;
      }
    }
  }
}

vector<Variable *> FunTerm::computeVars()
{
  vector<Variable *> result;
  for (int i = 0; i < len(arguments); ++i) {
    vector<Variable *> temp = arguments[i]->vars();
    append(result, temp);
  }
  return result;
}

string FunTerm::toString()
{
  assert(len(function->arguments) == len(arguments));
  int n = len(function->arguments);

  ostringstream oss;
  if (n) {
    oss << "(";
  }
  oss << function->name;
  for (int i = 0; i < n; ++i) {
    oss << " " << arguments[i]->toString();
  }
  if (n) {
    oss << ")";
  }
  return oss.str();
}

Z3_ast FunTerm::toSmt()
{
  assert(len(function->arguments) == len(arguments));
  //  int n = len(function->arguments);
  assert(function->hasInterpretation);
  
  if (isExistsFunction(function)) {
    // assert(n == 2);
    // oss << "exists ";
    // oss << "((";
    // oss << arguments[0]->toSmtString();
    // oss << " ";
    // assert(arguments[0].isVariable());
    // VarTerm *t = (VarTerm *)arguments[0];
    // assert(t->variable->sort->hasInterpretation);
    // oss << t->variable->sort->interpretation;
    // oss << ")) ";
    // oss << arguments[1]->toSmtString();
    // TODO
    assert(0);
    return function->interpretation(arguments);
  } else {
    Log(DEBUG8) << "Descending into " << function->name << endl;
    assert(function->hasInterpretation);
    Z3_ast result = function->interpretation(arguments);
    Log(DEBUG8) << "Ascending into " << function->name << endl;
    return result;
  }

  // assert(len(function->arguments) == len(arguments));
  // int n = len(function->arguments);

  // ostringstream oss;
  // if (n) {
  //   oss << "(";
  // }
  // assert(function->hasInterpretation);
  // if (isExistsFunction(function)) {
  //   assert(n == 2);
  //   oss << "exists ";
  //   oss << "((";
  //   oss << arguments[0]->toSmtString();
  //   oss << " ";
  //   assert(arguments[0].isVariable());
  //   VarTerm *t = (VarTerm *)arguments[0];
  //   assert(t->variable->sort->hasInterpretation);
  //   oss << t->variable->sort->interpretation;
  //   oss << ")) ";
  //   oss << arguments[1]->toSmtString();
  // } else {
  //   oss << function->interpretation;
  //   for (int i = 0; i < n; ++i) {
  //     oss << " " << arguments[i]->toSmtString();
  //   }
  // }
  // if (n) {
  //   oss << ")";
  // }
  // return oss.str();
}

string FunTerm::toPrettyString()
{
  assert(len(function->arguments) == len(arguments));
  int n = len(function->arguments);

  ostringstream oss;
  oss << function->name;
  if (n) {
    oss << "(";
  }
  for (int i = 0; i < n; ++i) {
    oss << arguments[i]->toPrettyString() << (i == n - 1 ? ")" : ",");
  }
  return oss.str();
}

bool FunTerm::computeIsNormalized(RewriteSystem &rewriteSystem, map<Term *, bool> &cache)
{
  if (!contains(cache, (Term *)this)) {
    for (int i = 0; i < len(rewriteSystem); ++i) {
      Term *l = rewriteSystem[i].first;
      Substitution subst;
      if (this->isInstanceOf(l, subst)) {
	      return cache[this] = false;
      }
    }
    for (int i = 0; i < len(arguments); ++i) {
      if (!arguments[i]->isNormalized(rewriteSystem)) {
	      return cache[this] = false;
      }
    }
    cache[this] = true;
  }
  return cache[this];
}

Term *FunTerm::computeSubstitution(Substitution &subst, map<Term *, Term *> &cache)
{
  if (!contains(cache, (Term *)this)) {
    vector<Term *> newargs;
    for (int i = 0; i < len(arguments); ++i) {
      newargs.push_back(arguments[i]->computeSubstitution(subst, cache));
    }
    cache[this] = getFunTerm(function, newargs);
  }
  return cache[this];
}

Term *FunTerm::computeNormalize(RewriteSystem &rewriteSystem, map<Term *, Term *> &cache, bool optimallyReducing)
{
  Log(DEBUG9) << "computeNormalizing " << this->toString() << endl;
  if (!contains(cache, (Term *)this)) {
    vector<Term *> subterms;
    for (int i = 0; i < len(function->arguments); ++i) {
      subterms.push_back(arguments[i]->computeNormalize(rewriteSystem, cache, optimallyReducing));
    }
    Term *result = getFunTerm(function, subterms);
    bool done = false;
    while (!done) {
      done = true;
      for (int i = 0; i < len(rewriteSystem); ++i) {
	      pair<Term *, Term *> rewriteRule = rewriteSystem[i];
	      Term *l = rewriteRule.first;
	      Term *r = rewriteRule.second;
	      Log(DEBUG9) << "Testing " << result->toString() << " with rewrite rule " << l->toString() << " ==> " << r->toString() << endl;

	      Substitution subst;
	      if (result->isInstanceOf(l, subst)) {
	        Log(DEBUG9) << "Is instance " << result->toString() << " of " << l->toString() << endl;
	        result = r->substitute(subst);
	        if (!optimallyReducing) {
	          result = result->computeNormalize(rewriteSystem, cache, optimallyReducing);
	        }
	        done = false;
	      }
      }
    }
    cache[this] = result;
    assert(result->isNormalized(rewriteSystem));
  }
  return cache[this];
}

bool FunTerm::unifyWith(Term *t, Substitution &subst)
{
  Log(DEBUG9) << "FunTerm::unifyWith " << this->toString() << " " << t->toString() << subst.toString() << endl;
  return t->unifyWithFunTerm(this, subst);
}

bool FunTerm::unifyWithVarTerm(VarTerm *t, Substitution &subst)
{
  Log(DEBUG9) << "FunTerm::unifyWithVarTerm " << this->toString() << " " << t->toString() << subst.toString() << endl;
  return t->unifyWithFunTerm(this, subst);
}

bool FunTerm::unifyWithFunTerm(FunTerm *t, Substitution &subst)
{
  Log(DEBUG9) << "FunTerm::unifyWithFunTerm " << this->toString() << " " << t->toString() << subst.toString() << endl;
  if (this->function == t->function) {
    for (int i = 0; i < len(arguments); ++i) {
      if (!this->arguments[i]->unifyWith(t->arguments[i], subst)) {
	      return false;
      }
    }
    return true;
  } else {
    return false;
  }
}

bool FunTerm::isVarTerm()
{
  return false;
}

VarTerm *FunTerm::getAsVarTerm()
{
  assert(0);
  return 0;
}

bool FunTerm::isFunTerm()
{
  return true;
}

FunTerm *FunTerm::getAsFunTerm()
{
  return (FunTerm *)this;
}

bool FunTerm::computeIsInstanceOf(Term *t, Substitution &s, map<pair<Term *, Term *>, bool> &cache)
{
  return t->computeIsGeneralizationOf(this, s, cache);
}

bool FunTerm::computeIsGeneralizationOf(VarTerm *, Substitution &, map<pair<Term *, Term *>, bool> &)
{
  return false;
}

bool FunTerm::computeIsGeneralizationOf(FunTerm *t, Substitution &s, map<pair<Term *, Term *>, bool> &cache)
{
  Log(DEBUG9) << "Testing is " << this->toString() << " is a generalization of " << t->toString() << endl;
  if (!contains(cache, make_pair((Term *)t, (Term *)this))) {
    if (this->function != t->function) {
      Log(DEBUG9) << "Nope, not same function symbol" << endl;
      cache[make_pair(t, this)] = false;
    } else {
      bool result = true;
      for (int i = 0; i < len(t->arguments); ++i) {
	      if (!t->arguments[i]->computeIsInstanceOf(arguments[i], s, cache)) {
	        result = false;
	        Log(DEBUG9) << "Nope, argument " << i << " failed." << endl;
	        break;
	      }
      }
      Log(DEBUG9) << "Yeap it is." << endl;
      cache[make_pair(t, this)] = result;
      if (result) {
	      assert(this->substitute(s) == t);
      }
    }
  }
  return cache[make_pair(t, this)];
}

int FunTerm::computeDagSize(map<Term *, int> &cache)
{
  if (contains(cache, (Term *)this)) {
    return 0;
  } else {
    int result = 1;
    for (int i = 0; i < len(arguments); ++i) {
      result += arguments[i]->computeDagSize(cache);
    }
    cache[this] = result;
    return result;
  }
}

Term *FunTerm::rewriteOneStep(RewriteSystem &rewrite, Substitution &how)
{
  // try a topmost rewrite
  Term *t = this->rewriteTopMost(rewrite, how);
  if (t != this) {
    return t;
  } 

  // now try inner terms
  vector<Term *> subterms;
  bool done = false;
  for (int i = 0; i < len(function->arguments); ++i) {
    Term *t = arguments[i]->rewriteOneStep(rewrite, how);
    subterms.push_back(done ? arguments[i] : t);
    done = t != arguments[i];
  }
  Term *result = getFunTerm(function, subterms);
  return result;
}

Term *FunTerm::rewriteOneStep(pair<Term *, Term *> rewriteRule, Substitution &how)
{
  // try a topmost rewrite
  Term *t = this->rewriteTopMost(rewriteRule, how);
  if (t != this) {
    return t;
  } 

  // now try inner terms
  vector<Term *> subterms;
  bool done = false;
  for (int i = 0; i < len(function->arguments); ++i) {
    Term *t = arguments[i]->rewriteOneStep(rewriteRule, how);
    subterms.push_back(done ? arguments[i] : t);
    done = t != arguments[i];
  }
  Term *result = getFunTerm(function, subterms);
  return result;
}

Term *FunTerm::rewriteOneStep(ConstrainedRewriteSystem &crs, Substitution &how)
{
  Log(DEBUG8) << "Term *FunTerm::rewriteOneStep(ConstrainedRewriteSystem &crs, Substitution &how)" << endl;
  
  // try a topmost rewrite
  Term *t = this->rewriteTopMost(crs, how);
  if (t != this) {
    return t;
  }

  // now try inner terms
  vector<Term *> subterms;
  bool done = false;
  for (int i = 0; i < len(function->arguments); ++i) {
    Term *t = arguments[i]->rewriteOneStep(crs, how);
    subterms.push_back(done ? arguments[i] : t);
    done = t != arguments[i];
  }
  Term *result = getFunTerm(function, subterms);
  return result;
}

Term *FunTerm::rewriteOneStep(pair<ConstrainedTerm, Term *> crewriteRule, Substitution &how)
{
  // try a topmost rewrite
  Term *t = this->rewriteTopMost(crewriteRule, how);
  if (t != this && isSatisfiable(crewriteRule.first.constraint->substitute(how)) == sat) {
    // TODO does not work when constraint has variables not in the lhs of the rewrite rule
    // to solve this issue, should add to "how" the variables occuring in the constraint but not the rule
    return t;
  }

  // now try inner terms
  vector<Term *> subterms;
  bool done = false;
  for (int i = 0; i < len(function->arguments); ++i) {
    Term *t = arguments[i]->rewriteOneStep(crewriteRule, how);
    subterms.push_back(done ? arguments[i] : t);
    done = t != arguments[i];
  }
  Term *result = getFunTerm(function, subterms);
  return result;
}

Term *FunTerm::abstract(Substitution &substitution)
{
  if (this->function->hasInterpretation) {
    Variable *var = createFreshVariable(this->function->result);
    assert(!substitution.inDomain(var));
    substitution.add(var, this);
    Term *result = getVarTerm(var);
    return result;
  } else {
    vector<Term *> abstractedArguments;
    for (int i = 0; i < (int)arguments.size(); ++i) {
      abstractedArguments.push_back(arguments[i]->abstract(substitution));
    }
    Term *result = getFunTerm(this->function, abstractedArguments);
    return result;
  }
}

vector<ConstrainedSolution> FunTerm::rewriteSearch(RewriteSystem &rs)
{
  vector<ConstrainedSolution> solutions;

  // top-most rewrite search
  for (int i = 0; i < len(rs); ++i) {
    pair<Term *, Term *> rewriteRule = rs[i];
    Term *l = rewriteRule.first;
    Term *r = rewriteRule.second;
    
    Substitution subst;
    if (this->isInstanceOf(l, subst)) {
      Term *term = r->substitute(subst);
      solutions.push_back(ConstrainedSolution(term, subst, l));
    }
  }

  // inner rewrite search
  for (int i = 0; i < len(function->arguments); ++i) {
    vector<ConstrainedSolution> innerSolutions = arguments[i]->rewriteSearch(rs);
    for (int j = 0; j < (int)innerSolutions.size(); ++j) {
      vector<Term *> newArguments;
      for (int k = 0; k < len(function->arguments); ++k) {
	      newArguments.push_back(arguments[k]);
      }
      newArguments[i] = innerSolutions[j].term;
      solutions.push_back(ConstrainedSolution(getFunTerm(function, newArguments),
				   innerSolutions[i].subst,
				   innerSolutions[i].lhsTerm));
    }
  }

  return solutions;
}

// caller needs to ensure freshness of rewrite system
vector<ConstrainedSolution> FunTerm::narrowSearch(ConstrainedRewriteSystem &crs)
{
  Log(DEBUG7) << "FunTerm::narrowSearch (crs) " << this->toString() << endl;
  vector<ConstrainedSolution> solutions;

  // top-most narrowing search
  for (int i = 0; i < len(crs); ++i) {
    pair<ConstrainedTerm, Term *> rewriteRule = crs[i];
    ConstrainedTerm l = rewriteRule.first;
    Term *r = rewriteRule.second;

    Substitution subst;
    Log(DEBUG8) << "Trying to unify " << this->toString() << " with " << l.term->toString() << endl;
    if (this->unifyWith(l.term, subst)) {
      Log(DEBUG8) << "Unification succeeded." << endl;
      Term *term = r;
      solutions.push_back(ConstrainedSolution(term, l.constraint, subst, l.term));
    } else {
      Log(DEBUG8) << "Unification failed." << endl;
    }
  }

  // inner narrowing search
  for (int i = 0; i < len(function->arguments); ++i) {
    vector<ConstrainedSolution> innerSolutions = arguments[i]->narrowSearch(crs);
    for (int j = 0; j < (int)innerSolutions.size(); ++j) {
      vector<Term *> newArguments;
      for (int k = 0; k < len(function->arguments); ++k) {
	      newArguments.push_back(arguments[k]);
      }
      newArguments[i] = innerSolutions[j].term;
      solutions.push_back(ConstrainedSolution(getFunTerm(function, newArguments),
					      innerSolutions[j].constraint,
					      innerSolutions[j].subst,
					      innerSolutions[j].lhsTerm));
    }
  }

  return solutions;
}

Sort *FunTerm::getSort()
{
  return this->function->result;
}
