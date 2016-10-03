#include "helper.h"
#include <algorithm>
#include "term.h"
#include "variable.h"
#include "substitution.h"
#include "factories.h"
#include <sstream>
#include <iostream>

using namespace std;

map<Variable *, Variable *> createRenaming(vector<Variable *>v, string s)
{
  map<Variable *, Variable *> result;

  int counter = 0;
  for (int i = 0; i < len(v); ++i) {
    if (contains(result, v[i])) {
      continue;
    }
    ostringstream oss;
    oss << s << counter;
    result[v[i]] = getInternalVariable(oss.str(), v[i]->sort);
    counter++;
  }

  return result;
}

Substitution createSubstitution(map<Variable *, Variable *> r)
{
  Substitution subst;
  for (map<Variable *, Variable *>::iterator it = r.begin();
       it != r.end(); ++it) {
    subst.add(it->first, getVarTerm(it->second));
  }
  return subst;
}

string varVecToString(vector<Variable *> vars)
{
  ostringstream oss;
  for (int i = 0; i < vars.size(); ++i) {
    oss << vars[i]->name << " ";
  }
  return oss.str();
}
