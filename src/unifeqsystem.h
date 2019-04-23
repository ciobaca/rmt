#ifndef UNIFEQSYSTEM_H__
#define UNIFEQSYSTEM_H__

#include <vector>
#include "unifeq.h"

struct UnifEqSystem : std::vector<UnifEq> {
  std::vector<UnifEq> eqSys;

  UnifEqSystem(Term *t1, Term *t2);
  UnifEqSystem(const UnifEq &eq);
  UnifEqSystem(const UnifEqSystem &ues);
};

#endif
