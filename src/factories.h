#ifndef FACTORIES_H__
#define FACTORIES_H__

#include <vector>
#include <string>
#include <map>
#include "term.h"
#include "constrainedrewritesystem.h"

struct Function;
struct Variable;
struct Name;
struct Term;
struct Sort;

Term *bImplies(Term *, Term *);
Term *bAnd(Term *, Term *);
Term *bAndVector(std::vector<Term *>, int = 0);
Term *bOrVector(std::vector<Term *>, int = 0);
Term *bOr(Term *, Term *);
Term *bNot(Term *);
Term *bTrue();
Term *bFalse();
Term *bExists(Variable *, Term *);
Term *bForall(Variable *, Term *);
bool isExistsFunction(Function *);
bool isForallFunction(Function *);
bool isQuantifierFunction(Function *);

Term *bEquals(Term *, Term *);
Term *mEquals(Term *, Term *);
Term *mle(Term *, Term *);
Term *mplus(Term *, Term *);
Term *mPlusVector(std::vector<Term *>, int = 0);
Term *mminus(Term *, Term *);
Term *mdiv(Term *, Term *);
Term *mmod(Term *, Term *);
Term *mtimes(Term *, Term *);
Term *mTimesVector(std::vector<Term *>, int = 0);

Function *getEqualsFunction(Sort *);
Term *createEqualityConstraint(Term *, Term *);

map< pair< Sort*, pair<Sort*, Sort*> >, Function* > *getSelectFunctions();
map< pair< Sort*, pair<Sort*, Sort*> >, Function* > *getStoreFunctions();

void createBuiltinExistsFunction(Sort*);
void createBuiltinForallFunction(Sort*);

void createBuiltIns();

RewriteSystem &getRewriteSystem(std::string name);
bool existsRewriteSystem(std::string name);
void putRewriteSystem(std::string name, RewriteSystem rewrite);

ConstrainedRewriteSystem &getConstrainedRewriteSystem(std::string name);
bool existsConstrainedRewriteSystem(std::string name);
void putConstrainedRewriteSystem(std::string name, ConstrainedRewriteSystem crewrite);

Variable *getVariable(std::string name);
void createVariable(std::string name, Sort *sort);

Sort *getSort(std::string name);
Sort *getBuiltinSort(Z3_string sortName);
Sort *getIntSort();
Sort *getBoolSort();
void createUninterpretedSort(const std::string &sortName);
void createInterpretedSort(const std::string &sortName, const std::string &interpretation);

Variable *getInternalVariable(std::string name, Sort *);

Function *getFunction(std::string name);
Function *getMinusFunction();
Function *getLEFunction();
Term *getIntOneConstant();
Term *getIntZeroConstant();
void updateDefinedFunction(std::string name, ConstrainedRewriteSystem &);

void createUninterpretedFunction(std::string name, std::vector<Sort *> arguments, Sort *result, bool isCommutative = false, bool isAssociative = false, Function *unityElement = NULL);
void createInterpretedFunction(std::string name, std::vector<Sort *> arguments, Sort *result, std::string interpretation);
void createInterpretedFunction(std::string name, std::vector<Sort *> arguments, Sort *result, Z3_func_decl);

Name *getName(std::string name);
void createName(std::string name);

Term *getFunTerm(Function *f, std::vector<Term *> arguments);
Term *getVarTerm(Variable *v);
Term *getNamTerm(Name *n);

bool variableExists(std::string);
bool sortExists(std::string);
bool isBuiltinSort(std::string);

Variable *createFreshVariable(Sort *);

std::vector<Variable *> getInterpretedVariables();

std::vector<Term *> vector1(Term *term);
std::vector<Term *> vector2(Term *term1, Term *term2);
std::vector<Term *> vector3(Term *term1, Term *term2, Term *term3);

Term *introduceExists(Term *, std::vector<Variable *>);

std::map<Variable *, Variable *> freshRenaming(std::vector<Variable *>);

Term *simplifyTerm(Term *);
Term *simplifyConstraint(Term *);
ConstrainedTerm simplifyConstrainedTerm(ConstrainedTerm);

#endif
