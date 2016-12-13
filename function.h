#ifndef FUNCTION_H__
#define FUNCTION_H__

#include <string>
#include <vector>

struct Sort;

using namespace std;

struct Function
{
  string name;
  vector<Sort *> arguments;
  Sort *result;

  bool hasInterpretation; // should be interpreted as a builtin function
  string interpretation;  // the name of the function in Z3

  bool isDefined; // is a function (not just a function symbol) defined by a set of rewrite rules in the "functions" rewrite system

private:
  Function(string name, vector<Sort *> arguments, Sort *result, bool isDefined)
  {
    this->name = name;
    this->arguments = arguments;
    this->result = result;
    this->hasInterpretation = false;
    this->isDefined = isDefined;
  }

  Function(string name, vector<Sort *> arguments, Sort *result, string interpretation)
  {
    this->name = name;
    this->arguments = arguments;
    this->result = result;
    this->hasInterpretation = true;
    this->interpretation = interpretation;
    this->isDefined = false;
  }

  friend void createUninterpretedFunction(string, vector<Sort *>, Sort *, bool);
  friend void createInterpretedFunction(string, vector<Sort *>, Sort *, string);
};

#endif
