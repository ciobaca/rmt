#include "fastterm.h"
#include <cassert>

bool matchList(FastTerm *tl1, FastTerm *tl2, uint32 count, FastSubst &subst)
{
  for (uint i = 0; i < count; ++i) {
    if (!match(tl1[i], tl2[i], subst)) {
      return false;
    }
  }
  return true;
}

bool match(FastTerm t1, FastTerm t2, FastSubst &subst)
{
  //  t1 = subst.applySubst(t1);
  t2 = subst.applySubst(t2);
  if (isFuncTerm(t1) && isFuncTerm(t2)) {
    if (getFunc(t1) != getFunc(t2)) {
      return false;
    }
    return matchList(args(t1), args(t2), getArity(getFunc(t1)), subst);
  } else {
    if (!isVariable(t2)) {
      return false;
    }
    if (t1 == t2) {
      return true;
    }
    // t2 is a variable
    // t1 is a term not equal to t2 (might be another variable)
    // if (occurs(t2, t1)) {
    //   return false;
    // }
    subst.composeWith(t1, t2);
    return true;
  }
}
