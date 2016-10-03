#include "constrainedrewrite.h"
#include "term.h"
#include "substitution.h"
#include "log.h"
#include "variable.h"
#include "factories.h"
#include "helper.h"
#include <cassert>
#include <string>
#include <sstream>

using namespace std;

void CRewriteSystem::addRule(ConstrainedTerm l, Term *r)
{
  //  assert(subseteq(r->vars(), l->vars()));
  this->push_back(make_pair(l, r));
}

// .vars
// .substitute

CRewriteSystem CRewriteSystem::rename(string s)
{
  vector<Variable *> vars;
  for (int i = 0; i < (int)this->size(); ++i) {
    append(vars, (*this)[i].first.vars());
    append(vars, (*this)[i].second->vars());
  }

  map<Variable *, Variable *> r = createRenaming(vars, s);
  Substitution subst = createSubstitution(r);

  CRewriteSystem result;
  for (int i = 0; i < (int)this->size(); ++i) {
    ConstrainedTerm l = (*this)[i].first;
    Term *r = (*this)[i].second;
    result.push_back(make_pair(l.substitute(subst), r->substitute(subst)));
  }

  return result;
}

CRewriteSystem CRewriteSystem::fresh(vector<Variable *> vars)
{
  Log(DEBUG8) << "Creating fresh rewrite system" << endl;
  static int counter = 0;
  vector<Variable *> myvars;
  for (int i = 0; i < (int)this->size(); ++i) {
    append(myvars, (*this)[i].first.vars());
    append(myvars, (*this)[i].second->vars());
  }
  Log(DEBUG8) << "Variables: " << varVecToString(myvars) << endl;

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
  }
  counter++;

  Substitution subst = createSubstitution(renaming);

  CRewriteSystem result;
  for (int i = 0; i < (int)this->size(); ++i) {
    ConstrainedTerm l = (*this)[i].first;
    Term *r = (*this)[i].second;
    result.push_back(make_pair(l.substitute(subst), r->substitute(subst)));
  }

  return result;
}

string CRewriteSystem::toString()
{
  ostringstream oss;
  for (int i = 0; i < (int)this->size(); ++i) {
    ConstrainedTerm l = (*this)[i].first;
    Term *r = (*this)[i].second;
    oss << l.toString() << " => " << r->toString();
    if (i != this->size() - 1) {
      oss << ", ";
    }
  }
  oss << ";";
  return oss.str();
}
