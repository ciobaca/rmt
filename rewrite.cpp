#include "rewrite.h"
#include "term.h"
#include "substitution.h"
#include "variable.h"
#include "factories.h"
#include "log.h"
#include <cassert>
#include <string>
#include <sstream>

using namespace std;

void RewriteSystem::addRule(Term *l, Term *r)
{
  assert(subseteq(r->vars(), l->vars()));
  this->push_back(make_pair(l, r));
}

RewriteSystem RewriteSystem::rename(string s)
{
  vector<Variable *> vars;
  for (int i = 0; i < (int)this->size(); ++i) {
    append(vars, (*this)[i].first->vars());
    append(vars, (*this)[i].second->vars());
  }

  map<Variable *, Variable *> r = createRenaming(vars, s);
  Substitution subst = createSubstitution(r);

  RewriteSystem result;
  for (int i = 0; i < (int)this->size(); ++i) {
    Term *l = (*this)[i].first;
    Term *r = (*this)[i].second;
    result.push_back(make_pair(l->substitute(subst), r->substitute(subst)));
  }

  return result;
}

RewriteSystem RewriteSystem::fresh(vector<Variable *> vars)
{
  static int counter = 0;
  vector<Variable *> myvars;
  for (int i = 0; i < (int)this->size(); ++i) {
    append(myvars, (*this)[i].first->vars());
    append(myvars, (*this)[i].second->vars());
  }
  Log(DEBUG9) << "Variables in rewrite system: " << endl;
  for (int i = 0; i < (int)myvars.size(); ++i) {
    Log(DEBUG9) << myvars[i]->name << " ";
  }

  map<Variable *, Variable *> renaming;
  
  for (int i = 0; i < len(myvars); ++i) {
    if (contains(renaming, myvars[i])) {
      continue;
    }
    ostringstream oss;
    oss << myvars[i]->name << "_" << counter;
    string varname = oss.str();
    createVariable(varname, myvars[i]->sort);
    renaming[myvars[i]] = getVariable(varname);
    Log(DEBUG9) << "Renaming " << myvars[i]->name << " to " << getVariable(varname)->name << endl;
  }
  counter++;

  Substitution subst = createSubstitution(renaming);

  Log(DEBUG9) << "Created substitution " << subst.toString() << endl;

  RewriteSystem result;
  for (int i = 0; i < (int)this->size(); ++i) {
    Term *l = (*this)[i].first;
    Term *r = (*this)[i].second;
    result.push_back(make_pair(l->substitute(subst), r->substitute(subst)));
  }

  return result;
}

string RewriteSystem::toString()
{
  ostringstream oss;
  for (int i = 0; i < (int)this->size(); ++i) {
    Term *l = (*this)[i].first;
    Term *r = (*this)[i].second;
    oss << l->toString() << " => " << r->toString();
    if (i != (int)this->size() - 1) {
      oss << ", ";
    }
  }
  oss << ";";
  return oss.str();
}
