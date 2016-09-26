#ifndef SOLUTION_H__
#define SOLUTION_H__

#include <string>
//#include <sstream>
//#include "term.h"
#include "substitution.h"

struct Term;
//struct Substitution;

struct Solution
{
  Term *term;
  Substitution substitution;

  Solution(Term *term, Substitution substitution);

  std::string toString();
};

#endif
