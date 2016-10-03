#ifndef CONSTRAINEDSOLUTION_H__
#define CONSTRAINEDSOLUTION_H__

#include <string>
//#include <sstream>
//#include "term.h"
#include "substitution.h"

struct Term;
//struct Substitution;

struct ConstrainedSolution
{
  Term *term;
  Substitution substitution;
  Term *constraint;

  ConstrainedSolution(Term *term, Substitution substitution, Term *constraint);

  std::string toString();
};

#endif
