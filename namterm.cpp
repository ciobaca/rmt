// it seems that we do not need names anymore
// #include "term.h"
// #include "namterm.h"
// #include "funterm.h"
// #include "varterm.h"
// #include "factories.h"
// #include <cassert>

// using namespace std;

// NamTerm::NamTerm(Name *name)
// {
//   this->name = name;
// }

// vector<Variable *> NamTerm::computeVars()
// {
//   vector<Variable *> result;
//   return result;
// }

// vector<Name *> NamTerm::names()
// {
//   vector<Name *> result;
//   result.push_back(name);
//   return result;
// }

// Term *NamTerm::computeSubstitution(Substitution &subst, map<Term *, Term *> &cache)
// {
//   return this;
// }

// string NamTerm::toString()
// {
//   return name->name;
// }

// bool NamTerm::unifyWith(Term *t, Substitution &subst)
// {
//   logmgu("NamTerm::unifyWith", this, t, subst);
//   return t->unifyWithNamTerm(this, subst);
// }

// bool NamTerm::unifyWithVarTerm(VarTerm *t, Substitution &subst)
// {
//   logmgu("NamTerm::unifyWithNamTerm", this, t, subst);
//   return t->unifyWithNamTerm(this, subst);
// }

// bool NamTerm::unifyWithFunTerm(FunTerm *t, Substitution &subst)
// {
//   logmgu("NamTerm::unifyWithFunTerm", this, t, subst);
//   return false;
// }

// bool NamTerm::unifyWithNamTerm(NamTerm *t, Substitution &subst)
// {
//   logmgu("NamTerm::unifyWithNamTerm", this, t, subst);
//   return this->name == t->name;
// }

// bool NamTerm::isVariable()
// {
//   return false;
// }

// vector<pair<Term *, Term *> > NamTerm::split()
// {
//   vector<pair<Term *, Term *> > result;
//   result.push_back(make_pair(getVarTerm(getVariable("\\_")), this));
//   return result;
// }

// bool NamTerm::computeIsNormalized(RewriteSystem &, map<Term *, bool> &)
// {
//   return true;
// }

// Term *NamTerm::computeNormalize(RewriteSystem &, map<Term *, Term *> &)
// {
//   return this;
// }

// bool NamTerm::computeIsInstanceOf(Term *t, Substitution &s, map<pair<Term *, Term *>, bool> &cache)
// {
//   return t->computeIsGeneralizationOf(this, s, cache);
// }

// bool NamTerm::computeIsGeneralizationOf(NamTerm *t, Substitution &s, map<pair<Term *, Term *>, bool> &cache)
// {
//   return this == t;
// }

// bool NamTerm::computeIsGeneralizationOf(VarTerm *t, Substitution &s, map<pair<Term *, Term *>, bool> &cache)
// {
//   return false;
// }

// bool NamTerm::computeIsGeneralizationOf(FunTerm *t, Substitution &s, map<pair<Term *, Term *>, bool> &cache)
// {
//   return false;
// }

// int NamTerm::computeDagSize(map<Term *, int> &cache)
// {
//   if (contains(cache, (Term *)this)) {
//     return 0;
//   } else {
//     cache[this] = 1;
//     return 1;
//   }
// }

// Term *NamTerm::rewriteOneStep(RewriteSystem &rewrite)
// {
//   return this->rewriteTopMost(rewrite);
// }
