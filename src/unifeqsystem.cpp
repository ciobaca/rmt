#include <algorithm>
#include "unifeqsystem.h"

UnifEqSystem::UnifEqSystem(Term *t1, Term *t2) {
  if (t1->isFunTerm && t2->isVarTerm) {
    std::swap(t1, t2);
  }
  this->emplace_back(t1, t2);
}

UnifEqSystem::UnifEqSystem(const UnifEq &eq) {
  this->push_back(eq);
}

UnifEqSystem::UnifEqSystem(const UnifEqSystem &ues) {
  (*this) = ues;
  std::sort(this->begin(), this->end(), [](const UnifEq &a, const UnifEq &b) {
    return a.t1->isVarTerm + a.t2->isVarTerm < b.t1->isVarTerm + b.t2->isVarTerm;
  });
}

void UnifEqSystem::decomp(FunTerm *t1, FunTerm *t2) {
  int n = t1->arguments.size();
  for (int i = 0; i < n; ++i) {
    this->push_back(UnifEq(t1->arguments[i], t2->arguments[i]));
  }
  std::sort(this->begin(), this->end(), [](const UnifEq &a, const UnifEq &b) {
    return a.t1->isVarTerm + a.t2->isVarTerm < b.t1->isVarTerm + b.t2->isVarTerm;
  });
}