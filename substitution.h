#ifndef SUBSTITUTION_H__
#define SUBSTITUTION_H__

#include <map>
#include <string>

struct Term;
struct Variable;

struct Substitution : public std::map<Variable *, Term *>
{
  Substitution();
  void apply(Substitution &);
  void force(Variable *v, Term *t);
  void add(Variable *v, Term *t);
  bool inDomain(Variable *v);
  bool inRange(Variable *v);
  Term *image(Variable *v);
  std::string toString();
};

#endif
