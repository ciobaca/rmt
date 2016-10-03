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

vector<ConstrainedTerm> ConstrainedTerm::smtNarrowSearch(CRewriteSystem &crs)
{
  return term->smtNarrowSearch(crs, constraint);
}

ConstrainedTerm ConstrainedTerm::normalize(RewriteSystem &rs)
{
  ConstrainedTerm ct(this->term->normalize(rs), this->constraint->normalize(rs));
  // ct.term = this->term->normalize(rs);
  // if (this->constraint) {
  //   ct.constraint = this->constraint->normalize(rs);
  // } else {
  //   ct.constraint = 0;
  // }
  return ct;
}

vector<Variable *> ConstrainedTerm::vars()
{
  vector<Variable *> result;
  append(result, term->vars());
  append(result, constraint->vars());
  return result;
}

ConstrainedTerm ConstrainedTerm::substitute(Substitution &subst)
{
  ConstrainedTerm ct(term->substitute(subst), constraint->substitute(subst));
  return ct;
}

