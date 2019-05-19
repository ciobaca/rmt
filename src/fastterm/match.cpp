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

bool match(FastTerm subject, FastTerm pattern, FastSubst &subst)
{
  if (isFuncTerm(subject) && isFuncTerm(pattern)) {
    if (getFunc(subject) != getFunc(pattern)) {
      return false;
    }
    return matchList(args(subject), args(pattern),
		     getArity(getFunc(subject)), subst);
  } else {
    if (!isVariable(pattern)) {
      return false;
    }
    assert(isVariable(pattern));
    if (subst.inDomain(pattern)) {
      return eq_term(subject, subst.range(pattern));
    } else {
      subst.composeWith(pattern, subject);
      return true;
    }
  }
}
