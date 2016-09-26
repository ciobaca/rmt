#include <map>
#include <sstream>
#include <cassert>
#include "sort.h"
#include "funterm.h"
#include "varterm.h"
//#include "namterm.h"
#include "factories.h"
#include "helper.h"

using namespace std;

map<string, RewriteSystem> rewriteSystems;
map<string, Variable *> variables;
map<string, Sort *> sorts;
map<string, Function *> functions;
//map<string, Name *> names;
map<pair<Function *, vector<Term *> >, Term *> funTerms;
map<Variable *, Term *> varTerms;
//map<Name *, Term *> namTerms;

RewriteSystem &getRewriteSystem(string name)
{
  return rewriteSystems[name];
}

bool existsRewriteSystem(string name)
{
  return rewriteSystems.find(name) != rewriteSystems.end();
}

void putRewriteSystem(string name, RewriteSystem rewrite)
{
  rewriteSystems[name] = rewrite;
}

Variable *getVariable(string name)
{
  if (contains(variables, name)) {
    return variables[name];
  } else {
    return 0;
  }
}

void createVariable(string name, Sort *sort)
{
#ifndef NDEBUG
  Variable *v = getVariable(name);
  assert(!v);
#endif

  variables[name] = new Variable(name, sort);
  //  fprintf(stderr, "Creating variable %s of sort %s.\n", name.c_str(), sort->name.c_str());
}

bool variableExists(string name)
{
  return contains(variables, name);
}

Sort *getSort(string name)
{
  if (contains(sorts, name)) {
    return sorts[name];
  } else {
    return 0;
  }
}

void createUninterpretedSort(string &sortName)
{
#ifndef NDEBUG
  Sort *s = getSort(sortName);
  assert(!s);
#endif
  
  sorts[sortName] = new Sort(sortName);
  fprintf(stderr, "Creating uninterpreted sort %s.\n", sortName.c_str());
}

void createInterpretedSort(string &sortName, string &interpretation)
{
#ifndef NDEBUG
  Sort *s = getSort(sortName);
  assert(!s);
#endif
  
  sorts[sortName] = new Sort(sortName, interpretation);
  fprintf(stderr, "Creating interpreted sort %s (as %s).\n", sortName.c_str(), interpretation.c_str());
}

bool sortExists(string name)
{
  return contains(sorts, name);
}

Variable *getInternalVariable(string name, Sort *sort)
{
  if (!getVariable(name)) {
    createVariable(name, sort);
  }
  assert(getVariable(name)->sort == sort);
  return getVariable(name);
}

Variable *createFreshVariable(Sort *sort)
{
  static int number = 0;
  ostringstream oss;
  oss << "_" << number++;
  string freshName = oss.str();
  createVariable(freshName, sort);
  return getVariable(freshName);
}

Function *getFunction(string name)
{
  if (contains(functions, name)) {
    return functions[name];
  } else {
    return 0;
  }
}

void createUninterpretedFunction(string name, vector<Sort *> arguments, Sort *result)
{
#ifndef NDEBUG
  Function *f = getFunction(name);
  assert(f == 0);
#endif
  int arity = arguments.size();
  fprintf(stderr, "Creating uninterpreted function %s : ", name.c_str());
  for (int i = 0; i < arity; ++i) {
    fprintf(stderr, "%s ", arguments[i]->name.c_str());
  }
  fprintf(stderr, " -> %s\n", result->name.c_str());
  functions[name] = new Function(name, arguments, result);
}

void createInterpretedFunction(string name, vector<Sort *> arguments, Sort *result, string interpretation)
{
#ifndef NDEBUG
  Function *f = getFunction(name);
  assert(f == 0);
#endif
  int arity = arguments.size();
  fprintf(stderr, "Creating interpreted function %s (as %s) : ", name.c_str(), interpretation.c_str());
  for (int i = 0; i < arity; ++i) {
    fprintf(stderr, "%s ", arguments[i]->name.c_str());
  }
  fprintf(stderr, " -> %s\n", result->name.c_str());
  functions[name] = new Function(name, arguments, result, interpretation);
}

// Name *getName(string name)
// {
//   if (contains(names, name)) {
//     return names[name];
//   }
//   return 0;
// }

// void createName(string name)
// {
// #ifndef NDEBUG
//   Name *n = getName(name);
//   assert(n == 0);
// #endif

//   names[name] = new Name(name);
// }

Term *getFunTerm(Function *f, vector<Term *> arguments)
{
  pair<Function *, vector<Term *> > content = make_pair(f, arguments);
  if (contains(funTerms, content)) {
    return funTerms[content];
  } else {
    return funTerms[content] = new FunTerm(f, arguments);
  }
}

Term *getVarTerm(Variable *v)
{
  if (contains(varTerms, v)) {
    return varTerms[v];
  } else {
    return varTerms[v] = new VarTerm(v);
  }
}

// Term *getNamTerm(Name *n)
// {
//   if (contains(namTerms, n)) {
//     return namTerms[n];
//   } else {
//     return namTerms[n] = new NamTerm(n);
//   }
// }

vector<Variable *> getInterpretedVariables()
{
  vector<Variable *> result;
  for (map<string, Variable *>::iterator it = variables.begin(); it != variables.end(); ++it) {
    if (it->second->sort->hasInterpretation) {
      result.push_back(it->second);
    }
  }
  return result;
}

vector<Term *> vector1(Term *term)
{
  vector<Term *> result;
  result.push_back(term);
  return result;
}

vector<Term *> vector2(Term *term1, Term *term2)
{
  vector<Term *> result;
  result.push_back(term1);
  result.push_back(term2);
  return result;
}
