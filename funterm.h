#ifndef FUNTERM_H__
#define FUNTERM_H__

#include "term.h"
#include "solution.h"

struct FunTerm : public Term
{
  Function *function;
  vector<Term *> arguments;

  FunTerm(Function *function, vector<Term *> arguments);

  virtual vector<Variable *> computeVars();
  //  virtual vector<Name *> names();

  virtual Sort *getSort();

  virtual string toString();

  virtual Term *computeSubstitution(Substitution &, map<Term *, Term *> &);

  virtual bool unifyWith(Term *, Substitution &);
  virtual bool unifyWithFunTerm(FunTerm *, Substitution &);
  virtual bool unifyWithVarTerm(VarTerm *, Substitution &);
  //  virtual bool unifyWithNamTerm(NamTerm *, Substitution &);

  virtual bool isVariable();

  virtual vector<pair<Term *, Term *> > split();

  virtual bool computeIsNormalized(RewriteSystem &, map<Term *, bool> &);
  virtual Term *computeNormalize(RewriteSystem &, map<Term *, Term *> &);

  virtual bool computeIsInstanceOf(Term *, Substitution &, map<pair<Term *, Term *>, bool> &);
  //  virtual bool computeIsGeneralizationOf(NamTerm *, Substitution &, map<pair<Term *, Term *>, bool> &);
  virtual bool computeIsGeneralizationOf(VarTerm *, Substitution &, map<pair<Term *, Term *>, bool> &);
  virtual bool computeIsGeneralizationOf(FunTerm *, Substitution &, map<pair<Term *, Term *>, bool> &);

  virtual int computeDagSize(map<Term *, int> &);

  virtual Term *rewriteOneStep(RewriteSystem &, Substitution &how);
  virtual Term *rewriteOneStep(pair<Term *, Term *>, Substitution &how);

  virtual Term *abstract(Substitution &);

  virtual string toSmtString();

  virtual vector<Solution> rewriteSearch(RewriteSystem &);
  virtual vector<Solution> narrowSearch(RewriteSystem &);
  virtual vector<ConstrainedSolution> narrowSearch(CRewriteSystem &);
};

#endif
