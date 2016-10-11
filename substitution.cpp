#include "substitution.h"
#include "helper.h"
#include "term.h"
#include <cassert>
#include <sstream>

using namespace std;

Substitution::Substitution()
{
}

void Substitution::add(Variable *v, Term *t)
{
  (*this)[v] = t;
}

void Substitution::apply(Substitution &s)
{
  for (Substitution::iterator it = this->begin(); it != this->end(); ++it) {
    it->second = it->second->substitute(s);
  }
}

void Substitution::force(Variable *v, Term *t)
{
  Substitution temp;
  temp.add(v, t);
  apply(temp);
  add(v, t);
}

bool Substitution::inDomain(Variable *v)
{
  return contains(*this, v);
}

Term *Substitution::image(Variable *v)
{
  assert(contains(*this, v));
  return (*this)[v];
}

string Substitution::toString()
{
  ostringstream oss;
  oss << "( ";
  for (Substitution::iterator it = this->begin(); it != this->end(); ++it) {
    oss << it->first->name << " |-> " << it->second->toString() << " ";
  }
  oss << ")";
  return oss.str();
}
