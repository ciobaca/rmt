#ifndef Z3_DRIVER__
#define Z3_DRIVER__

#include <vector>
#include <string>

struct Variable;
struct Term;

enum Z3Result
{
  sat,
  unsat,
  unknown
};

struct Z3Theory
{
  std::vector<Variable *> variables;
  std::vector<Term *> constraints;
  
  void addVariable(Variable *);
  void addEqualityConstraint(Term *, Term *);
  void addConstraint(Term *);

  Z3Result isSatisfiable();
};

Z3Result isSatisfiable(Term *constraint);

void startz3api();

#endif
