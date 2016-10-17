#include "funterm.h"
#include "varterm.h"
#include "log.h"
#include "namterm.h"
#include "factories.h"
#include "helper.h"
#include "sort.h"
#include <sstream>
#include <cassert>
#include <iostream>

using namespace std;

FunTerm::FunTerm(Function *function, vector<Term *> arguments)
{
  assert(this->arguments->size() == function->arguments.size());
  this->function = function;
  this->arguments = arguments;
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

// vector<Name *> FunTerm::names()
// {
//   vector<Name *> result;
//   for (int i = 0; i < len(arguments); ++i) {
//     vector<Name *> temp = arguments[i]->names();
//     append(result, temp);
//   }
//   return result;
// }

string FunTerm::toString()
{
  assert(len(function->arguments) == len(arguments));
  int n = len(function->arguments);

  ostringstream oss;
  oss << function->name;
  if (n) {
    oss << "(";
  }
  for (int i = 0; i < n; ++i) {
    oss << arguments[i]->toString() << (i == n - 1 ? ")" : ",");
  }
  return oss.str();
}

string FunTerm::toSmtString()
{
  assert(len(function->arguments) == len(arguments));
  int n = len(function->arguments);

  ostringstream oss;
  if (n) {
    oss << "(";
  }
  assert(function->hasInterpretation);
  if (isExistsFunction(function)) {
    assert(n == 2);
    oss << "exists ";
    oss << "((";
    oss << arguments[0]->toSmtString();
    oss << " ";
    assert(arguments[0].isVariable());
    VarTerm *t = (VarTerm *)arguments[0];
    oss << t->variable->sort->name;
    oss << ")) ";
    oss << arguments[1]->toSmtString();
  } else {
    oss << function->interpretation;
    for (int i = 0; i < n; ++i) {
      oss << " " << arguments[i]->toSmtString();
    }
  }
  if (n) {
    oss << ")";
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

Term *FunTerm::computeNormalize(RewriteSystem &rewriteSystem, map<Term *, Term *> &cache)
{
  if (!contains(cache, (Term *)this)) {
    vector<Term *> subterms;
    for (int i = 0; i < len(function->arguments); ++i) {
      subterms.push_back(arguments[i]->computeNormalize(rewriteSystem, cache));
    }
    Term *result = getFunTerm(function, subterms);
    bool done = false;
    while (!done) {
      done = true;
      for (int i = 0; i < len(rewriteSystem); ++i) {
	pair<Term *, Term *> rewriteRule = rewriteSystem[i];
	Term *l = rewriteRule.first;
	Term *r = rewriteRule.second;

	Substitution subst;
	if (result->isInstanceOf(l, subst)) {
	  result = r->substitute(subst);
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

// bool FunTerm::unifyWithNamTerm(NamTerm *t, Substitution &subst)
// {
//   logmgu("FunTerm::unifyWithNamTerm", this, t, subst);
//   return false;
// }

bool FunTerm::isVariable()
{
  return false;
}

vector<pair<Term *, Term *> > FunTerm::split()
{
  vector<pair<Term *, Term *> > result;

  result.push_back(make_pair(getVarTerm(getVariable("\\_")), this));
  for (int i = 0; i < len(arguments); ++i) {
    vector<pair<Term *, Term *> > temp = arguments[i]->split();
    for (int j = 0; j < len(temp); ++j) {
      Term *context = temp[j].first;
      Term *hole = temp[j].second;

      vector<Term *> newArguments;
      for (int k = 0; k < i; ++k) {
	newArguments.push_back(arguments[k]);
      }
      newArguments.push_back(context);
      for (int k = i + 1; k < len(arguments); ++k) {
	newArguments.push_back(arguments[k]);
      }
      result.push_back(make_pair(getFunTerm(function, newArguments), hole));
    }
  }
  return result;
}

bool FunTerm::computeIsInstanceOf(Term *t, Substitution &s, map<pair<Term *, Term *>, bool> &cache)
{
  return t->computeIsGeneralizationOf(this, s, cache);
}

// bool FunTerm::computeIsGeneralizationOf(NamTerm *t, Substitution &s, map<pair<Term *, Term *>, bool> &cache)
// {
//   return false;
// }

bool FunTerm::computeIsGeneralizationOf(VarTerm *t, Substitution &s, map<pair<Term *, Term *>, bool> &cache)
{
  return false;
}

bool FunTerm::computeIsGeneralizationOf(FunTerm *t, Substitution &s, map<pair<Term *, Term *>, bool> &cache)
{
  if (!contains(cache, make_pair((Term *)t, (Term *)this))) {
    if (this->function != t->function) {
      cache[make_pair(t, this)] = false;
    } else {
      bool result = true;
      for (int i = 0; i < len(t->arguments); ++i) {
	if (!t->arguments[i]->computeIsInstanceOf(arguments[i], s, cache)) {
	  result = false;
	  break;
	}
      }
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

Term *FunTerm::abstract(Substitution &substitution)
{
  // fprintf(stderr, "abstracting %s.\n", this->toString().c_str());
  if (this->function->hasInterpretation) {
    Variable *var = createFreshVariable(this->function->result);
    assert(!substitution.inDomain(var));
    //    fprintf(stderr, "Successfully created abstraction variable.\n");
    substitution.add(var, this);
    //    fprintf(stderr, "Successfully updated substitution.\n");
    Term *result = getVarTerm(var);
    //    fprintf(stderr, "Successfully created abstracted term.\n");
    return result;
  } else {
    vector<Term *> abstractedArguments;
    for (int i = 0; i < arguments.size(); ++i) {
      //      fprintf(stderr, "abstracting argument %d.n\n", i);
      abstractedArguments.push_back(arguments[i]->abstract(substitution));
      //      fprintf(stderr, "finished abstracting argument %d.n\n", i);
    }
    Term *result = getFunTerm(this->function, abstractedArguments);
    //    fprintf(stderr, "finished abstracting %s.n\n", this->toString().c_str());
    return result;
  }
}

vector<Solution> FunTerm::rewriteSearch(RewriteSystem &rs)
{
  vector<Solution> solutions;

  // top-most rewrite search
  for (int i = 0; i < len(rs); ++i) {
    pair<Term *, Term *> rewriteRule = rs[i];
    Term *l = rewriteRule.first;
    Term *r = rewriteRule.second;
    
    Substitution subst;
    if (this->isInstanceOf(l, subst)) {
      Term *term = r->substitute(subst);
      solutions.push_back(Solution(term, subst));
    }
  }

  // inner rewrite search
  for (int i = 0; i < len(function->arguments); ++i) {
    vector<Solution> innerSolutions = arguments[i]->rewriteSearch(rs);
    for (int j = 0; j < innerSolutions.size(); ++j) {
      vector<Term *> newArguments;
      for (int k = 0; k < len(function->arguments); ++k) {
	newArguments.push_back(arguments[k]);
      }
      newArguments[i] = innerSolutions[j].term;
      solutions.push_back(Solution(getFunTerm(function, newArguments),
				   innerSolutions[i].substitution));
    }
  }

  return solutions;
}

// caller needs to ensure freshness of rewrite system
vector<Solution> FunTerm::narrowSearch(RewriteSystem &rs)
{
  Log(DEBUG7) << "FunTerm::narrowSearch(RewriteSystem &rs)" << this->toString() << endl;
  vector<Solution> solutions;

  // top-most narrowing search
  for (int i = 0; i < len(rs); ++i) {
    pair<Term *, Term *> rewriteRule = rs[i];
    Term *l = rewriteRule.first;
    Term *r = rewriteRule.second;
    Log(DEBUG7) << "Top-mosting with " << l->toString() << " => " << r->toString() << endl;

    Substitution subst;
    Log(DEBUG7) << "Initial subst " << subst.toString() << endl;
    //    cerr << "Trying to unify " << this->toString() << " with " << l->toString() << endl;
    if (this->unifyWith(l, subst)) {
      //      cerr << "Unification succeeded." << endl;
      Term *term = r;
      solutions.push_back(Solution(term, subst));
    } else {
      //      cerr << "Unification failed." << endl;
    }
  }

  // inner narrowing search
  for (int i = 0; i < len(function->arguments); ++i) {
    Log(DEBUG7) << "Innering with " << arguments[i]->toString() << endl;
    vector<Solution> innerSolutions = arguments[i]->narrowSearch(rs);
    for (int j = 0; j < innerSolutions.size(); ++j) {
      vector<Term *> newArguments;
      for (int k = 0; k < len(function->arguments); ++k) {
	newArguments.push_back(arguments[k]);
      }
      newArguments[i] = innerSolutions[j].term;
      solutions.push_back(Solution(getFunTerm(function, newArguments),
				   innerSolutions[j].substitution));
    }
  }
  Log(DEBUG7) << "Done FunTerm::narrowSearch(RewriteSystem &rs) " << this->toString() << endl;
  return solutions;
}

// caller needs to ensure freshness of rewrite system
vector<ConstrainedSolution> FunTerm::narrowSearch(CRewriteSystem &crs)
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
      solutions.push_back(ConstrainedSolution(term, subst, l.constraint));
    } else {
      Log(DEBUG8) << "Unification failed." << endl;
    }
  }

  // inner narrowing search
  for (int i = 0; i < len(function->arguments); ++i) {
    vector<ConstrainedSolution> innerSolutions = arguments[i]->narrowSearch(crs);
    for (int j = 0; j < innerSolutions.size(); ++j) {
      vector<Term *> newArguments;
      for (int k = 0; k < len(function->arguments); ++k) {
	newArguments.push_back(arguments[k]);
      }
      newArguments[i] = innerSolutions[j].term;
      solutions.push_back(ConstrainedSolution(getFunTerm(function, newArguments),
				   innerSolutions[i].substitution, innerSolutions[i].constraint));
    }
  }

  return solutions;
}

Sort *FunTerm::getSort()
{
  return this->function->result;
}
