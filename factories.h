#ifndef FACTORIES_H__
#define FACTORIES_H__

#include <vector>
#include <string>
#include <map>
#include "term.h"
#include "constrainedrewrite.h"

struct Function;
struct Variable;
struct Name;
struct Term;
struct Sort;

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

RewriteSystem &getRewriteSystem(std::string name);
bool existsRewriteSystem(std::string name);
void putRewriteSystem(std::string name, RewriteSystem rewrite);

CRewriteSystem &getCRewriteSystem(std::string name);
bool existsCRewriteSystem(std::string name);
void putCRewriteSystem(std::string name, CRewriteSystem crewrite);

Variable *getVariable(std::string name);
void createVariable(std::string name, Sort *sort);

Sort *getSort(std::string name);
void createUninterpretedSort(std::string &sortName);
void createInterpretedSort(std::string &sortName, std::string &interpretation);

Variable *getInternalVariable(std::string name, Sort *);

Function *getFunction(std::string name);
void createUninterpretedFunction(std::string name, std::vector<Sort *> arguments, Sort *result);
void createInterpretedFunction(std::string name, std::vector<Sort *> arguments, Sort *result, std::string interpretation);

Name *getName(std::string name);
void createName(std::string name);

Term *getFunTerm(Function *f, std::vector<Term *> arguments);
Term *getVarTerm(Variable *v);
Term *getNamTerm(Name *n);

bool variableExists(std::string name);
bool sortExists(std::string name);

Variable *createFreshVariable(Sort *);

std::vector<Variable *> getInterpretedVariables();

std::vector<Term *> vector1(Term *term);
std::vector<Term *> vector2(Term *term1, Term *term2);

Term *simplifyConstraint(Term *);
ConstrainedTerm simplifyConstrainedTerm(ConstrainedTerm);

Term *introduceExists(Term *, std::vector<Variable *>);

std::map<Variable *, Variable *> freshRenaming(std::vector<Variable *>);

#endif
