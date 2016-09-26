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
  bool hasInterpretation;
  string interpretation;

private:
  Function(string name, vector<Sort *> arguments, Sort *result)
  {
    this->name = name;
    this->arguments = arguments;
    this->result = result;
    this->hasInterpretation = false;
  }

  Function(string name, vector<Sort *> arguments, Sort *result, string interpretation)
  {
    this->name = name;
    this->arguments = arguments;
    this->result = result;
    this->hasInterpretation = true;
    this->interpretation = interpretation;
  }

  friend void createUninterpretedFunction(string, vector<Sort *>, Sort *);
  friend void createInterpretedFunction(string, vector<Sort *>, Sort *, string);
};

#endif
