#include <cassert>
#include "varterm.h"
#include "funterm.h"
#include "log.h"
#include "sort.h"
#include "helper.h"
#include "factories.h"

using namespace std;

VarTerm::VarTerm(Variable *variable)
{
  this->variable = variable;
}

vector<Variable *> VarTerm::computeVars()
{
  vector<Variable *> result;
  result.push_back(variable);
  return result;
}

// vector<Name *> VarTerm::names()
// {
//   vector<Name *> result;
//   return result;
// }

Term *VarTerm::computeSubstitution(Substitution &subst, map<Term *, Term *> &)
{
  if (subst.inDomain(variable)) {
    return subst.image(variable);
  } else {
    return this;
  }
}

string VarTerm::toString()
{
  return variable->name;
}

string VarTerm::toSmtString()
{
  return variable->name;
}

bool VarTerm::unifyWith(Term *t, Substitution &subst)
{
  logmgu("VarTerm::unifyWith", this, t, subst);
  return t->unifyWithVarTerm(this, subst);
}

bool VarTerm::unifyWithVarTerm(VarTerm *t, Substitution &subst)
{
  logmgu("VarTerm::unifyWithVarTerm", this, t, subst);
  if (this == t) {
    return true;
  }
  if (subst.inDomain(this->variable)) {
    if (subst.inDomain(t->variable)) {
      return subst.image(this->variable)->unifyWith(subst.image(t->variable), subst);
    } else {
      return subst.image(this->variable)->unifyWithVarTerm(t, subst);
    }
  } else {
    if (subst.inDomain(t->variable)) {
      return subst.image(t->variable)->unifyWithVarTerm(this, subst);
    } else {
      if (this->variable->sort->hasSubSortTR(t->variable->sort)){
	subst.force(this->variable, t);
      } else if (t->variable->sort->hasSubSortTR(this->variable->sort)){
	subst.force(t->variable, this);
      } else {
	// trying to unify two variables of incompatible sorts
	return false;
      }
    }
    return true;
  }
}

bool VarTerm::unifyWithFunTerm(FunTerm *t, Substitution &subst)
{
  logmgu("VarTerm::unifyWithFunTerm", this, t, subst);
  vector<Variable *> ocVars = t->vars();

  if (subst.inDomain(this->variable)) {
    return subst.image(this->variable)->unifyWithFunTerm(t, subst);
  } else if (contains(ocVars, this->variable)) {
    return false;
  } else {
    if (this->variable->sort->hasSubSortTR(t->function->result)) {
      subst.force(this->variable, t);
      return true;
    } else {
      return false;
    }
  }
}

// bool VarTerm::unifyWithNamTerm(NamTerm *t, Substitution &subst)
// {
//   logmgu("VarTerm::unifyWithNamTerm", this, t, subst);
//   if (subst.inDomain(this->variable)) {
//     return subst.image(this->variable)->unifyWithNamTerm(t, subst);
//   } else {
//     subst.force(this->variable, t);
//     return true;
//   }
// }

bool VarTerm::isVariable()
{
  return true;
}

vector<pair<Term *, Term *> > VarTerm::split()
{
  vector<pair<Term *, Term *> > result;
  result.push_back(make_pair(getVarTerm(getVariable("\\_")), this));
  return result;
}

bool VarTerm::computeIsNormalized(RewriteSystem &, map<Term *, bool> &)
{
  return true;
}

Term *VarTerm::computeNormalize(RewriteSystem &, map<Term *, Term *> &)
{
  return this;
}

bool VarTerm::computeIsInstanceOf(Term *t, Substitution &s, map<pair<Term *, Term *>, bool> &cache)
{
  return t->computeIsGeneralizationOf(this, s, cache);
}

// bool VarTerm::computeIsGeneralizationOf(NamTerm *t, Substitution &s, map<pair<Term *, Term *>, bool> &cache)
// {
//   if (!contains(cache, make_pair((Term *)t, (Term *)this))) {
//     if (contains(s, this->variable)) {
//       cache[make_pair(t, this)] = s.image(this->variable) == t;
//     } else {
//       s.add(this->variable, t);
//       cache[make_pair(t, this)] = true;
//     }
//   }
//   return cache[make_pair(t, this)];
// }

bool VarTerm::computeIsGeneralizationOf(VarTerm *t, Substitution &s, map<pair<Term *, Term *>, bool> &cache)
{
  if (!contains(cache, make_pair((Term *)t, (Term *)this))) {
    if (contains(s, this->variable)) {
      cache[make_pair(t, this)] = s.image(this->variable) == t;
    } else {
      if (this->variable->sort->hasSubSortTR(t->variable->sort)) {
	s.add(this->variable, t);
	cache[make_pair(t, this)] = true;
      } else {
	cache[make_pair(t, this)] = false;
      }
    }
  }
  return cache[make_pair(t, this)];
}

bool VarTerm::computeIsGeneralizationOf(FunTerm *t, Substitution &s, map<pair<Term *, Term *>, bool> &cache)
{
  if (!contains(cache, make_pair((Term *)t, (Term *)this))) {
    if (contains(s, this->variable)) {
      cache[make_pair(t, this)] = s.image(this->variable) == t;
    } else {
      if (this->variable->sort->hasSubSortTR(t->function->result)) {
	//	fprintf(stderr, "%s has %s as subsort\n", this->variable->sort->name.c_str(), t->function->result->name.c_str());
	s.add(this->variable, t);
	cache[make_pair(t, this)] = true;
      } else {
	//	fprintf(stderr, "NOT %s has %s as subsort\n", this->variable->sort->name.c_str(), t->function->result->name.c_str());
	cache[make_pair(t, this)] = false;
      }
    }
  }
  return cache[make_pair(t, this)];
}

int VarTerm::computeDagSize(map<Term *, int> &cache)
{
  if (contains(cache, (Term *)this)) {
    return 0;
  } else {
    cache[this] = 1;
    return 1;
  }
}

Term *VarTerm::rewriteOneStep(RewriteSystem &rewrite, Substitution &how)
{
  return this->rewriteTopMost(rewrite, how);
}

Term *VarTerm::rewriteOneStep(pair<Term *, Term *> rewriteRule, Substitution &how)
{
  return this->rewriteTopMost(rewriteRule, how);
}

Term *VarTerm::abstract(Substitution &substitution)
{
  if (variable->sort->hasInterpretation) {
    Variable *var = createFreshVariable(this->variable->sort);
    assert(!substitution.inDomain(var));
    substitution.add(var, this);
    Term *result = getVarTerm(var);
    return result;
  } else {
    return this;
  }
}

vector<Solution> VarTerm::rewriteSearch(RewriteSystem &rs)
{
  vector<Solution> result;
  for (int i = 0; i < len(rs); ++i) {
    pair<Term *, Term *> rewriteRule = rs[i];
    Term *l = rewriteRule.first;
    Term *r = rewriteRule.second;
    
    Substitution subst;
    if (this->isInstanceOf(l, subst)) {
      Term *term = r->substitute(subst);
      result.push_back(Solution(term, subst));
    }
  }
  return result;
}

// caller needs to ensure freshness of rewrite system
vector<Solution> VarTerm::narrowSearch(RewriteSystem &rs)
{
  vector<Solution> result;
  for (int i = 0; i < len(rs); ++i) {
    pair<Term *, Term *> rewriteRule = rs[i];
    Term *l = rewriteRule.first;
    Term *r = rewriteRule.second;
    
    Substitution subst;
    if (this->unifyWith(l, subst)) {
      Term *term = r;
      result.push_back(Solution(term, subst));
    }
  }
  return result;
}

vector<ConstrainedSolution> VarTerm::narrowSearch(CRewriteSystem &crs)
{
  Log(DEBUG7) << "VarTerm::narrowSearch (crs) " << this->toString() << endl;
  vector<ConstrainedSolution> result;
  for (int i = 0; i < len(crs); ++i) {
    pair<ConstrainedTerm, Term *> crewriteRule = crs[i];
    ConstrainedTerm l = crewriteRule.first;
    Term *r = crewriteRule.second;

    Substitution subst;
    if (this->unifyWith(l.term, subst)) {
      Term *term = r;
      result.push_back(ConstrainedSolution(term, subst, l.constraint));
    }
  }
  return result;
}

Sort *VarTerm::getSort()
{
  return this->variable->sort;
}

