#include "helper.h"
#include <algorithm>
#include "term.h"
#include "variable.h"
#include "substitution.h"
#include "factories.h"
#include <sstream>
#include <iostream>

using namespace std;

void abortWithMessage(string error)
{
  cout << "Error: " << error << endl;
  exit(0);
}

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
  for (int i = 0; i < (int)vars.size(); ++i) {
    oss << vars[i]->name << " ";
  }
  return oss.str();
}

string spaces(int tabs)
{
  ostringstream oss;
  for (int i = 0; i < tabs; ++i) {
    oss << "    ";
  }
  return oss.str();
}

string string_from_int(int number)
{
  ostringstream oss;
  oss << number;
  return oss.str();
}

