#include "fastterm.h"
#include "fastqueryacunify.h"
#include <cassert>
#include <vector>

// bool unifyList(FastTerm *tl1, FastTerm *tl2, uint32 count, FastSubst &subst)
// {
//   for (uint i = 0; i < count; ++i) {
//     if (!unify(tl1[i], tl2[i]).size()) {
//       return false;
//     }
//   }
//   return true;
// }

std::vector<FastSubst> unify(FastTerm t1, FastTerm t2)
{
  return FastQueryACUnify(t1, t2).solve();
}
