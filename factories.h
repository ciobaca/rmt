#ifndef FACTORIES_H__
#define FACTORIES_H__

#include <vector>
#include <string>
#include "term.h"

struct Function;
struct Variable;
struct Name;
struct Term;
struct Sort;

using namespace std;

RewriteSystem &getRewriteSystem(string name);
bool existsRewriteSystem(string name);
void putRewriteSystem(string name, RewriteSystem rewrite);

Variable *getVariable(string name);
void createVariable(string name, Sort *sort);

Sort *getSort(string name);
void createUninterpretedSort(string &sortName);
void createInterpretedSort(string &sortName, string &interpretation);

Variable *getInternalVariable(string name, Sort *);

Function *getFunction(string name);
void createUninterpretedFunction(string name, vector<Sort *> arguments, Sort *result);
void createInterpretedFunction(string name, vector<Sort *> arguments, Sort *result, string interpretation);

Name *getName(string name);
void createName(string name);

Term *getFunTerm(Function *f, vector<Term *> arguments);
Term *getVarTerm(Variable *v);
Term *getNamTerm(Name *n);

bool variableExists(string name);
bool sortExists(string name);

Variable *createFreshVariable(Sort *);

vector<Variable *> getInterpretedVariables();

vector<Term *> vector1(Term *term);
vector<Term *> vector2(Term *term1, Term *term2);

#endif
