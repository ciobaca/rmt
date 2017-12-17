#ifndef VARTERM_H__
#define VARTERM_H__

#include "term.h"

struct VarTerm : public Term
{
  Variable *variable;

  VarTerm(Variable *variable);

  virtual vector<Variable *> computeVars();

  virtual Sort *getSort();

  virtual Term *computeSubstitution(Substitution &, map<Term *, Term *> &);

  virtual bool unifyWith(Term *, Substitution &);
  virtual bool unifyWithFunTerm(FunTerm *, Substitution &);
  virtual bool unifyWithVarTerm(VarTerm *, Substitution &);

  virtual bool isVarTerm();
  virtual bool isFunTerm();
  virtual VarTerm *getAsVarTerm();
  virtual FunTerm *getAsFunTerm();

  virtual bool computeIsNormalized(RewriteSystem &rewriteSystem, map<Term *, bool> &);
  virtual Term *computeNormalize(RewriteSystem &, map<Term *, Term *> &, bool);

  virtual bool computeIsInstanceOf(Term *, Substitution &, map<pair<Term *, Term *>, bool> &);
  virtual bool computeIsGeneralizationOf(VarTerm *, Substitution &, map<pair<Term *, Term *>, bool> &);
  virtual bool computeIsGeneralizationOf(FunTerm *, Substitution &, map<pair<Term *, Term *>, bool> &);

  virtual Term *rewriteOneStep(RewriteSystem &, Substitution &how);
  virtual Term *rewriteOneStep(pair<Term *, Term *>, Substitution &how);
  virtual Term *rewriteOneStep(ConstrainedRewriteSystem &, Substitution &how);
  virtual Term *rewriteOneStep(pair<ConstrainedTerm, Term *>, Substitution &how);

  virtual int computeDagSize(map<Term *, int> &);

  virtual Term *abstract(Substitution &);

  virtual vector<ConstrainedSolution> rewriteSearch(RewriteSystem &);
  virtual vector<ConstrainedSolution> narrowSearch(ConstrainedRewriteSystem &);

  virtual string toString();
  virtual string toSmtString();
  virtual string toPrettyString();
};

#endif
