#include "rewritesystem.h"
#include "fastterm.h"
#include "helper.h"
#include <cassert>
#include <string>
#include <sstream>

using namespace std;

void RewriteSystem::addRule(ConstrainedTerm l, FastTerm r)
{
  this->push_back(make_pair(l, r));
}

// RewriteSystem RewriteSystem::rename(string s)
// {
//   vector<Variable *> vars;
//   for (int i = 0; i < (int)this->size(); ++i) {
//     append(vars, (*this)[i].first.vars());
//     append(vars, (*this)[i].second->vars());
//   }

//   map<Variable *, Variable *> r = createRenaming(vars, s);
//   Substitution subst = createSubstitution(r);

//   RewriteSystem result;
//   for (int i = 0; i < (int)this->size(); ++i) {
//     ConstrainedTerm l = (*this)[i].first;
//     Term *r = (*this)[i].second;
//     result.push_back(make_pair(l.substitute(subst), r->substitute(subst)));
//   }

//   return result;
// }

// RewriteSystem RewriteSystem::fresh()
// {
//   Log(DEBUG8) << "Creating fresh rewrite system" << endl;
//   //  static int counter = 100;
//   vector<Variable *> myvars;
//   for (int i = 0; i < (int)this->size(); ++i) {
//     append(myvars, (*this)[i].first.vars());
//     append(myvars, (*this)[i].second->vars());
//   }
//   Log(DEBUG8) << "Variables: " << varVecToString(myvars) << endl;

//   map<Variable *, Variable *> renaming = freshRenaming(myvars);
  
//   Substitution subst = createSubstitution(renaming);

//   RewriteSystem result;
//   for (int i = 0; i < (int)this->size(); ++i) {
//     ConstrainedTerm l = (*this)[i].first;
//     Term *r = (*this)[i].second;
//     result.push_back(make_pair(l.substitute(subst), r->substitute(subst)));
//   }

//   return result;
// }

string toString(const RewriteSystem &rs)
{
  ostringstream oss;
  for (int i = 0; i < (int)rs.size(); ++i) {
    ConstrainedTerm l = rs[i].first;
    FastTerm r = rs[i].second;
    oss << ::toString(l) << " => " << toString(r);
    if (i != (int)rs.size() - 1) {
      oss << ", ";
    }
  }
  oss << ";";
  return oss.str();
}
