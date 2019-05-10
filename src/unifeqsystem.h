#ifndef UNIFEQSYSTEM_H__
#define UNIFEQSYSTEM_H__

#include <vector>
#include "unifeq.h"
#include "varterm.h"
#include "funterm.h"
#include "term.h"


struct UnifEqSystem : std::vector<UnifEq> {
  UnifEqSystem(Term *t1, Term *t2);
  UnifEqSystem(const UnifEq &eq);
  UnifEqSystem(const UnifEqSystem &ues);
  void decomp(FunTerm *t1, FunTerm *t2);
};

#endif