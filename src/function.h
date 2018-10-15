#ifndef FUNCTION_H__
#define FUNCTION_H__

#include <string>
#include <vector>
#include "z3driver.h"
#include "helper.h"

struct Sort;

using namespace std;

struct Function
{
  string name;
  vector<Sort *> arguments;
  Sort *result;

  bool hasInterpretation; // should be interpreted as a builtin function
  //string interpretation;  // the name of the function in Z3
  func_interpretation interpretation;

  bool isEqualityFunction;

  bool isDefined; // is a function (not just a function symbol) defined by a set of rewrite rules in the "functions" rewrite system

private:
  Function(string name, vector<Sort *> arguments, Sort *result, bool isDefined)
  {
    this->name = name;
    this->arguments = arguments;
    this->result = result;
    this->hasInterpretation = false;
    this->isDefined = isDefined;
    this->isEqualityFunction = false;
  }

  Function(string name, vector<Sort *> arguments, Sort *result, string interpretation)
  {
    this->name = name;
    this->arguments = arguments;
    this->result = result;
    this->hasInterpretation = true;
    if (interpretation == "+") {
      this->interpretation = z3_add;
    } else if (interpretation == "*") {
      this->interpretation = z3_mul;
    } else if (interpretation == "-") {
      this->interpretation = z3_sub;
    } else if (interpretation == "div") {
      this->interpretation = z3_div;
    } else if (interpretation == "mod") {
      this->interpretation = z3_mod;
    } else if (interpretation == "<=") {
      this->interpretation = z3_le;
    } else if (interpretation == "<") {
      this->interpretation = z3_lt;
    } else if (interpretation == "=") {
      this->interpretation = z3_eq;
      this->isEqualityFunction = true;
    } else if (interpretation == "0") {
      this->interpretation = z3_ct_0;
    } else if (interpretation == "1") {
      this->interpretation = z3_ct_1;
    } else if (interpretation == "2") {
      this->interpretation = z3_ct_2;
    } else if (interpretation == "3") {
      this->interpretation = z3_ct_3;
    } else if (interpretation == "4") {
      this->interpretation = z3_ct_4;
    } else if (interpretation == "5") {
      this->interpretation = z3_ct_5;
    } else if (interpretation == "6") {
      this->interpretation = z3_ct_6;
    } else if (interpretation == "7") {
      this->interpretation = z3_ct_7;
    } else if (interpretation == "8") {
      this->interpretation = z3_ct_8;
    } else if (interpretation == "9") {
      this->interpretation = z3_ct_9;
    } else if (interpretation == "10") {
      this->interpretation = z3_ct_10;
    } else if (interpretation == "11") {
      this->interpretation = z3_ct_11;
    } else if (interpretation == "12") {
      this->interpretation = z3_ct_12;
    } else if (interpretation == "13") {
      this->interpretation = z3_ct_13;
    } else if (interpretation == "14") {
      this->interpretation = z3_ct_14;
    } else if (interpretation == "15") {
      this->interpretation = z3_ct_15;
    } else if (interpretation == "true") {
      this->interpretation = z3_true;
    } else if (interpretation == "false") {
      this->interpretation = z3_false; 
    } else if (interpretation == "not") {
      this->interpretation = z3_not;
    } else if (interpretation == "and") {
      this->interpretation = z3_and;
    } else if (interpretation == "iff") {
      this->interpretation = z3_iff;
    } else if (interpretation == "or") {
      this->interpretation = z3_or;
    } else if (interpretation == "=>") {
      this->interpretation = z3_implies;
    } else {
      abortWithMessage(std::string("Unknown interpretation of function (") + interpretation + ").");
    }
    
    this->isDefined = false;
  }

  friend void createUninterpretedFunction(string, vector<Sort *>, Sort *, bool);
  friend void createInterpretedFunction(string, vector<Sort *>, Sort *, string);
};

#endif
