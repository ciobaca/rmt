#ifndef QUERYACUNIFY_H__
#define QUERYACUNIFY_H__

#include <string>
#include <vector>
#include <map>
#include "query.h"
#include "term.h"
#include "constrainedterm.h"
#include "substitution.h"
#include "unifeqsystem.h"

struct QueryACUnify : public Query {
  Term *t1;
  Term *t2;

  QueryACUnify();
  virtual Query *create();
  virtual void parse(std::string &s, int &w);
  virtual void execute();
  std::vector<Substitution> solve(UnifEqSystem ues);
  std::vector<Substitution> solveAC(UnifEq ues);
  std::vector<Substitution> solveACC(UnifEq ues);
  std::vector<Substitution> combineSubsts(const vector<Substitution> &substs1, const vector<Substitution> &substs2);
  bool solvedForm(const UnifEqSystem &ues);
  Substitution getSubstFromSolvedForm(const UnifEqSystem &ues);

private:
  void delSameCoeffs(std::map<Term*, int> &l, std::map<Term*, int> &r);
  std::vector<int> fromMapToVector(const std::map<Term*, int> &M);
  Term* createFuncWithSameVar(int cnt, Term *var, Function *f, Term *unityElement = nullptr);
};

#endif
