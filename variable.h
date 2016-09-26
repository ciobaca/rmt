#ifndef VARIABLE_H__
#define VARIABLE_H__

#include <string>

using namespace std;

struct Substitution;
struct Sort;

struct Variable
{
  Sort *sort;
  string name;
  Variable *rename(Substitution &);

private:
  Variable(string name, Sort *sort)
  {
    this->name = name;
    this->sort = sort;
  }

  friend void createVariable(string, Sort *);
};


#endif
