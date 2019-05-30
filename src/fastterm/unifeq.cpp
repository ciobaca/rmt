#include <algorithm>
#include "unifeq.h"
#include "fastterm.h"

UnifEq::UnifEq(FastTerm t1, FastTerm t2) {
  if (isFuncTerm(t1) && isVariable(t2)) {
    std::swap(t1, t2);
  }
  this->t1 = t1;
  this->t2 = t2;
}