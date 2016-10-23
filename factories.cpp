#include <map>
#include <sstream>
#include <cassert>
#include "sort.h"
#include "funterm.h"
#include "varterm.h"
#include "factories.h"
#include "helper.h"
#include "log.h"

using namespace std;

map<string, RewriteSystem> rewriteSystems;
map<string, CRewriteSystem> cRewriteSystems;
map<string, Variable *> variables;
map<string, Sort *> sorts;
map<string, Function *> functions;
//map<string, Name *> names;
map<pair<Function *, vector<Term *> >, Term *> funTerms;
map<Variable *, Term *> varTerms;

RewriteSystem &getRewriteSystem(string name)
{
  if (!existsRewriteSystem(name)) {
    Log(ERROR) << "Cannot find rewrite system " << name << endl;
    assert(0);
  }
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

CRewriteSystem &getCRewriteSystem(string name)
{
  return cRewriteSystems[name];
}

bool existsCRewriteSystem(string name)
{
  return cRewriteSystems.find(name) != cRewriteSystems.end();
}

void putCRewriteSystem(string name, CRewriteSystem crewrite)
{
  cRewriteSystems[name] = crewrite;
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
  Log(INFO) << "Creating uninterpreted sort " << sortName << "." << endl;
}

void createInterpretedSort(string &sortName, string &interpretation)
{
#ifndef NDEBUG
  Sort *s = getSort(sortName);
  assert(!s);
#endif
  
  sorts[sortName] = new Sort(sortName, interpretation);
  Log(INFO) << "Creating interpreted sort " << sortName << " as (" << interpretation << ")." << endl;
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
  Log log(INFO);
  log << "Creating uninterpreted function " << name << " : ";
  for (int i = 0; i < arity; ++i) {
    log << arguments[i]->name << " ";
  }
  log << " -> " << result->name << endl;
  functions[name] = new Function(name, arguments, result);
}

void createInterpretedFunction(string name, vector<Sort *> arguments, Sort *result, string interpretation)
{
#ifndef NDEBUG
  Function *f = getFunction(name);
  assert(f == 0);
#endif
  int arity = arguments.size();
  Log log(INFO);
  log << "Creating interpreted function " << name << " (as " << interpretation << ") : ";
  for (int i = 0; i < arity; ++i) {
    log << arguments[i]->name << " ";
  }
  log << " -> " << result->name << endl;
  assert(interpretation != "");
  functions[name] = new Function(name, arguments, result, interpretation);
}

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

vector<Term *> vector0()
{
  vector<Term *> result;
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

Function *TrueFun;
Function *FalseFun;
Function *NotFun;
Function *AndFun;
Function *ImpliesFun;
Function *OrFun;
Function *EqualsFun;

map<Sort *, Function *> ExistsFun;

void createBuiltIns()
{
  TrueFun = getFunction("true");
  assert(TrueFun);
  FalseFun = getFunction("false");
  assert(FalseFun);
  NotFun = getFunction("bnot");
  assert(NotFun);
  AndFun = getFunction("band");
  assert(AndFun);
  ImpliesFun = getFunction("bimplies");
  assert(ImpliesFun);
  OrFun = getFunction("bor");
  assert(OrFun);
  EqualsFun = getFunction("bequals");
  assert(EqualsFun);

  assert(sortExists("Bool"));
  
  // small hack for existential quantifiers
  Log(DEBUG) << "Creating built ins" << endl;
  for (map<string, Sort *>::iterator it = sorts.begin(); it != sorts.end(); ++it) {
    Sort *s = it->second;
    vector<Sort *> args;
    args.push_back(s);
    args.push_back(sorts["Bool"]);
    ostringstream funname;
    funname << "_exists" << s->name;
    Log(DEBUG) << "Creating exists function " << funname << endl;
    createUninterpretedFunction(funname.str(), args, sorts["Bool"]);
    ExistsFun[s] = getFunction(funname.str());
    assert(ExistsFun[s]);
  }
}

bool isExistsFunction(Function *f)
{
  for (map<Sort *, Function *>::iterator it = ExistsFun.begin(); it != ExistsFun.end(); ++it) {
    if (it->second == f) {
      return true;
    }
  }
  return false;
}

Term *bExists(Variable *var, Term *condition)
{
  assert(ExistsFun[var->sort]);
  Term *result = getFunTerm(ExistsFun[var->sort], vector2(getVarTerm(var), condition));
  return result;
}

Term *bImplies(Term *left, Term *right)
{
  return getFunTerm(ImpliesFun, vector2(left, right));
}

Term *bAnd(Term *left, Term *right)
{
  return getFunTerm(AndFun, vector2(left, right));
}

Term *bOr(Term *left, Term *right)
{
  return getFunTerm(OrFun, vector2(left, right));
}

Term *bNot(Term *term)
{
  return getFunTerm(NotFun, vector1(term));
}

Term *bTrue()
{
  return getFunTerm(TrueFun, vector0());
}

Term *bFalse()
{
  return getFunTerm(FalseFun, vector0());
}

Term *simplifyConstraint(Term *constraint)
{
  if (existsRewriteSystem("simplifications")) {
    RewriteSystem rs = getRewriteSystem("simplifications");
    Log(DEBUG9) << "Normalizing constraint " << constraint->toString() << endl;
    return constraint->normalize(rs);
  } else {
    assert(0);
  }
  return constraint;
}

ConstrainedTerm simplifyConstrainedTerm(ConstrainedTerm ct)
{
  if (existsRewriteSystem("simplifications")) {
    RewriteSystem rs = getRewriteSystem("simplifications");
    return ConstrainedTerm(ct.term->normalize(rs), ct.constraint->normalize(rs));
  } else {
    assert(0);
  }
  return ct;
}

Function *getEqualsFunction(Sort *sort)
{
  for (map<string, Function *>::iterator it = functions.begin(); it != functions.end(); ++it) {
    Function *f = it->second;
    if (f->arguments.size() != 2) {
      continue;
    }
    if (f->arguments[0] != sort) {
      continue;
    }
    if (f->arguments[1] != sort) {
      continue;
    }
    if (!f->hasInterpretation) {
      continue;
    }
    if (f->interpretation != "=") {
      continue;
    }
    return f;
  }
  return 0;
}

Term *createEqualityConstraint(Term *t1, Term *t2)
{
  assert(t1);
  assert(t2);
  Sort *s1 = t1->getSort();
  Sort *s2 = t2->getSort();
  if (s1 != s2) {
    assert(0);
  }
  Function *equalsFun = getEqualsFunction(s1);
  if (!equalsFun) {
    Log(ERROR) << "Cannot find equality function symbol for sort " << s1->name << endl;
    assert(0);
  }
  return getFunTerm(equalsFun, vector2(t1, t2));
}
