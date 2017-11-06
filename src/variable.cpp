#include "variable.h"
#include "substitution.h"
#include "term.h"
#include "varterm.h"
#include <cassert>

using namespace std;

Variable *Variable::rename(Substitution &subst)
{
  if (subst.inDomain(this)) {
    Term *image = subst.image(this);
    VarTerm *vt = dynamic_cast<VarTerm *>(image);
    assert(vt);
    return vt->variable;
  } else {
    return this;
  }
}
