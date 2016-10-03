#include "constrainedsolution.h"
#include <sstream>
#include "substitution.h"
#include "term.h"

using namespace std;

ConstrainedSolution::ConstrainedSolution(Term *term, Substitution substitution, Term *constraint) :
  term(term),
  substitution(substitution),
  constraint(constraint)
{
}

std::string ConstrainedSolution::toString() {
  std::ostringstream oss;
  oss << "term: " << term->toString() << std::endl;
  oss << "subst: ";
  for (Substitution::iterator it = substitution.begin(); it != substitution.end(); ++it) {
    oss << it->first->name << " |-> " << it->second->toString() << "; ";
  }
  oss << "constraint: " << constraint->toString() << std::endl;
  oss << std::endl;
  return oss.str();
}
