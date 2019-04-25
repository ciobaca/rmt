#ifndef FUNTERM_H__
#define FUNTERM_H__

#include "term.h"
#include "constrainedsolution.h"

struct FunTerm : public Term
{
  Function *function;
  vector<Term *> arguments;

  FunTerm(Function *function, vector<Term *> arguments);

  virtual vector<Variable *> computeVars();

  virtual Sort *getSort();

  virtual Term *computeSubstitution(Substitution &, map<Term *, Term *> &);

  virtual bool unifyWith(Term *, Substitution &);
  virtual bool unifyWithFunTerm(FunTerm *, Substitution &);
  virtual bool unifyWithVarTerm(VarTerm *, Substitution &);

  virtual VarTerm *getAsVarTerm();
  virtual FunTerm *getAsFunTerm();

  virtual bool computeIsNormalized(RewriteSystem &, map<Term *, bool> &);
  virtual Term *computeNormalize(RewriteSystem &, map<Term *, Term *> &, bool);

  virtual bool computeIsInstanceOf(Term *, Substitution &, map<pair<Term *, Term *>, bool> &);
  virtual bool computeIsGeneralizationOf(VarTerm *, Substitution &, map<pair<Term *, Term *>, bool> &);
  virtual bool computeIsGeneralizationOf(FunTerm *, Substitution &, map<pair<Term *, Term *>, bool> &);

  virtual int computeDagSize(map<Term *, int> &);

  virtual Term *rewriteOneStep(RewriteSystem &, Substitution &how);
  virtual Term *rewriteOneStep(pair<Term *, Term *>, Substitution &how);
  virtual Term *rewriteOneStep(ConstrainedRewriteSystem &, Substitution &how);
  virtual Term *rewriteOneStep(pair<ConstrainedTerm, Term *>, Substitution &how);

  virtual Term *abstract(Substitution &);

  virtual Z3_ast toSmt();

  virtual string toString();
  virtual string toPrettyString();

  virtual vector<ConstrainedSolution> rewriteSearch(RewriteSystem &);
  virtual vector<ConstrainedSolution> narrowSearch(ConstrainedRewriteSystem &);

  virtual Term *compute();

  virtual void getDefinedFunctions(std::set<Function *> &);
  virtual Term *unsubstitute(std::vector<Term *> cts, std::vector<Variable *> vs);
};

#endif
