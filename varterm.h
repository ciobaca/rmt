#ifndef VARTERM_H__
#define VARTERM_H__

#include "term.h"

struct VarTerm : public Term
{
  Variable *variable;

  VarTerm(Variable *variable);

  virtual string toString();

  virtual vector<Variable *> computeVars();
  //  virtual vector<Name *> names();

  virtual Sort *getSort();

  virtual Term *computeSubstitution(Substitution &, map<Term *, Term *> &);

  virtual bool unifyWith(Term *, Substitution &);
  virtual bool unifyWithFunTerm(FunTerm *, Substitution &);
  virtual bool unifyWithVarTerm(VarTerm *, Substitution &);
  //  virtual bool unifyWithNamTerm(NamTerm *, Substitution &);

  virtual bool isVariable();

  virtual vector<pair<Term *, Term *> > split();

  virtual bool computeIsNormalized(RewriteSystem &rewriteSystem, map<Term *, bool> &);
  virtual Term *computeNormalize(RewriteSystem &, map<Term *, Term *> &);

  virtual bool computeIsInstanceOf(Term *, Substitution &, map<pair<Term *, Term *>, bool> &);
  //  virtual bool computeIsGeneralizationOf(NamTerm *, Substitution &, map<pair<Term *, Term *>, bool> &);
  virtual bool computeIsGeneralizationOf(VarTerm *, Substitution &, map<pair<Term *, Term *>, bool> &);
  virtual bool computeIsGeneralizationOf(FunTerm *, Substitution &, map<pair<Term *, Term *>, bool> &);

  virtual Term *rewriteOneStep(RewriteSystem &, Substitution &how);
  virtual Term *rewriteOneStep(pair<Term *, Term *>, Substitution &how);

  virtual int computeDagSize(map<Term *, int> &);

  virtual Term *abstract(Substitution &);

  virtual vector<Solution> rewriteSearch(RewriteSystem &);
  virtual vector<Solution> narrowSearch(RewriteSystem &);
  virtual vector<ConstrainedSolution> narrowSearch(CRewriteSystem &);

  virtual string toSmtString();
};

#endif
