#include "fastterm.h"
#include <cassert>

bool unifyList(FastTerm *tl1, FastTerm *tl2, uint32 count, FastSubst &subst)
{
  for (uint i = 0; i < count; ++i) {
    if (!unify(tl1[i], tl2[i], subst)) {
      return false;
    }
  }
  return true;
}

bool unify(FastTerm t1, FastTerm t2, FastSubst &subst)
{
  t1 = subst.applySubst(t1);
  t2 = subst.applySubst(t2);
  if (isFuncTerm(t1) && isFuncTerm(t2)) {
    if (getFunc(t1) != getFunc(t2)) {
      return false;
    }
    return unifyList(args(t1), args(t2), getArity(getFunc(t1)), subst);
  } else {
    if (isFuncTerm(t1)) {
      FastTerm temp = t1;
      t1 = t2;
      t2 = temp;
    }
    assert(isVariable(t1));
    if (!isVariable(t2) && occurs(t1, t2)) {
      return false;
    }
    if (t1 != t2) {
      subst.composeWith(t1, t2);
    }
    return true;
  }
}
