#include "constrainedterm.h"
#include "term.h"
#include <sstream>

using namespace std;

string ConstrainedTerm::toString()
{
  ostringstream oss;
  oss << term->toString();
  if (constraint) {
    oss << " /\\ ";
    oss << constraint->toString();
  }
  return oss.str();
}

vector<ConstrainedTerm> ConstrainedTerm::smtNarrowSearch(RewriteSystem &rs)
{
  return term->smtNarrowSearch(rs, constraint);
}

ConstrainedTerm ConstrainedTerm::normalize(RewriteSystem &rs)
{
  ConstrainedTerm ct;
  ct.term = this->term->normalize(rs);
  if (this->constraint) {
    ct.constraint = this->constraint->normalize(rs);
  } else {
    ct.constraint = 0;
  }
  return ct;
}

