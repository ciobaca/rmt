#ifndef QUERYACUNIFY_H__
#define QUERYACUNIFY_H__

#include <string>
#include <vector>
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
  std::vector<Substitution> combineAllSubst(const vector<vector<Substitution>> &vecSubstLayer);
  bool solvedForm(const UnifEqSystem &ues);
  Substitution getSubstFromSolvedForm(const UnifEqSystem &ues);

private:
  bool checkElementarity(Term *t);
};

#endif
