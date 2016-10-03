#ifndef FACTORIES_H__
#define FACTORIES_H__

#include <vector>
#include <string>
#include "term.h"
#include "constrainedrewrite.h"

struct Function;
struct Variable;
struct Name;
struct Term;
struct Sort;

using namespace std;

Term *bImplies(Term *, Term *);
Term *bAnd(Term *, Term *);
Term *bOr(Term *, Term *);
Term *bNot(Term *);
Term *bTrue();
Term *bFalse();
Term *bExists(Variable *, Term *);
bool isExistsFunction(Function *);

Function *getEqualsFunction(Sort *);
Term *createEqualityConstraint(Term *, Term *);

void createBuiltIns();

RewriteSystem &getRewriteSystem(string name);
bool existsRewriteSystem(string name);
void putRewriteSystem(string name, RewriteSystem rewrite);

CRewriteSystem &getCRewriteSystem(string name);
bool existsCRewriteSystem(string name);
void putCRewriteSystem(string name, CRewriteSystem crewrite);

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

Term *simplifyConstraint(Term *);
ConstrainedTerm simplifyConstrainedTerm(ConstrainedTerm);

#endif
