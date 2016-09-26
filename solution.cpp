#include "solution.h"
#include <sstream>
#include "substitution.h"
#include "term.h"

using namespace std;

Solution::Solution(Term *term, Substitution substitution)
{
  this->term = term;
  this->substitution = substitution;
}

std::string Solution::toString() {
  std::ostringstream oss;
  oss << "term: " << term->toString() << std::endl;
  oss << "subst: ";
  for (Substitution::iterator it = substitution.begin(); it != substitution.end(); ++it) {
    oss << it->first->name << " |-> " << it->second->toString() << "; ";
  }
  oss << std::endl;
  return oss.str();
}
