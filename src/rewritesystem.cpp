#include "rewritesystem.h"
#include "fastterm.h"
#include "helper.h"
#include "log.h"
#include <cassert>
#include <string>
#include <sstream>
#include <iostream>

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

RewriteSystem renameAway(const RewriteSystem &rs, vector<FastVar> vars)
{
  LOG(DEBUG8, cerr << "Creating fresh rewrite system");
  vector<FastVar> myvars;
  for (uint i = 0; i < rs.size(); ++i) {
    append(myvars, uniqueVars(rs[i].first.term));
    append(myvars, uniqueVars(rs[i].first.constraint));
    append(myvars, uniqueVars(rs[i].second));
  }
  vector<FastVar> toRename;
  //  LOG(DEBUG8, cerr << "Variables: " << varVecToString(myvars) << endl;

  std::sort(myvars.begin(), myvars.end());
  auto it = std::unique(myvars.begin(), myvars.end());
  myvars.resize(std::distance(myvars.begin(), it));

  for (uint i = 0; i < myvars.size(); ++i) {
    if (contains(vars, myvars[i])) {
      toRename.push_back(myvars[i]);
    }
  }
  
  FastSubst subst;
  for (uint i = 0; i < toRename.size(); ++i) {
    FastVar var = createFreshVariable(getVarSort(toRename[i]));
    subst.addToSubst(toRename[i], var);
  }

  RewriteSystem result;
  for (uint i = 0; i < rs.size(); ++i) {
    FastTerm lhs = rs[i].first.term;
    FastTerm constraint = rs[i].first.constraint;
    FastTerm r = rs[i].second;
    result.push_back(make_pair(ConstrainedTerm(subst.applySubst(lhs), subst.applySubst(constraint)), subst.applySubst(r)));
  }
  return result;
}

string toString(const RewriteSystem &rs)
{
  ostringstream oss;
  for (uint i = 0; i < rs.size(); ++i) {
    ConstrainedTerm l = rs[i].first;
    FastTerm r = rs[i].second;
    oss << ::toString(l) << " => " << toString(r);
    if (i != rs.size() - 1) {
      oss << ", ";
    }
  }
  oss << ";";
  return oss.str();
}

FastTerm rewriteTopMost(FastTerm term, const RewriteSystem &rs, FastSubst &how, Z3_context context)
{
  RewriteSystem rsp = renameAway(rs, uniqueVars(term));
  for (uint i = 0; i < rsp.size(); ++i) {
    pair<ConstrainedTerm, FastTerm> rwRule = rsp[i];
    FastTerm result = rewriteTopMost(term, rwRule.first, rwRule.second, how, context);
    if (!eq_term(term, result)) {
      return result;
    }
  }
  return term;
}

FastTerm rewriteTopMost(FastTerm term, const ConstrainedTerm &clhs, const FastTerm &rhs,
    FastSubst &how, Z3_context context)
{
  FastTerm lhs = clhs.term;
  FastTerm constraint = clhs.constraint;

  LOG(DEBUG7, cerr << "Checking if " << toString(term) << " is instance of " << toString(lhs));
  vector<FastSubst> substs = match(term, lhs);
  for (uint i = 0; i < substs.size(); ++i){ 
    LOG(DEBUG7, cerr << toString(term) << " is instance of " << toString(lhs) << " w/ subst " << toString(substs[i]));
    LOG(DEBUG7, cerr << ": " << toString(substs[i].applySubst(constraint)));
    if (z3_sat_check(context, substs[i].applySubst(constraint)) != Z3_L_FALSE) {
      LOG(DEBUG7, cerr << "    satisfiable instance");
      // TODO does not work when constraint has variables not in the lhs of the rewrite rule
      // to solve this issue, should add to "how" the variables occuring in the constraint but not the rule
      how = substs[i];
      FastTerm result = substs[i].applySubst(rhs);
      LOG(DEBUG7, cerr << "result is " << toString(result));
      return result;
    } else {
      LOG(DEBUG7, cerr << "    not satisfiable instance");
    }
  }
  LOG(DEBUG8, cerr << "not an instance of " << toString(lhs));
  return term;
}
