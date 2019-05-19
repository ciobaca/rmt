#include "fastterm.h"
#include <cassert>

bool unifyHelper(FastTerm, FastTerm, FastSubst);

bool unifyHelperList(FastTerm *tl1, FastTerm *tl2, uint32 count, FastSubst subst)
{
  for (uint i = 0; i < count; ++i) {
    if (!unifyHelper(tl1[i], tl2[i], subst)) {
      return false;
    }
  }
  return true;
}

bool unifyHelper(FastTerm t1, FastTerm t2, FastSubst subst)
{
  t1 = applySubst(t1, subst);
  t2 = applySubst(t2, subst);
  if (isFuncTerm(t1) && isFuncTerm(t2)) {
    if (getFunc(t1) != getFunc(t2)) {
      return false;
    }
    return unifyHelperList(args(t1), args(t2), getArity(getFunc(t1)), subst);
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
      composeSubst(subst, t1, t2);
    }
    return true;
  }
}

bool unify(FastTerm t1, FastTerm t2, FastSubst *result)
{
  *result = newSubst();
  return unifyHelper(t1, t2, *result);
}
