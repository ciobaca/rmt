#include <algorithm>
#include "unifeqsystem.h"

using namespace std;

UnifEqSystem::UnifEqSystem(Term *t1, Term *t2) {
  if (t1->isFunTerm && t2->isVarTerm) {
    swap(t1, t2);
  }
  eqSys.emplace_back(t1, t2);
}

UnifEqSystem::UnifEqSystem(const UnifEq &eq) {
  eqSys.push_back(eq);
}

UnifEqSystem::UnifEqSystem(const UnifEqSystem &ues) {
  eqSys.insert(eqSys.end(), ues.begin(), ues.end());
  
}