#include "fastterm.h"
#include <stdio.h>
#include <cstring>
#include <assert.h>
#include <z3.h>
#include <stdlib.h>
#include <vector>
#include <sstream>

using namespace std;

void initFastTerm()
{
  initSorts();
  initFuncs();
}

uint32 termDataSize = 0;
uint32 termData[MAXDATA];

bool validFastVarTerm(FastTerm term)
{
  assert(validFastTerm(term));
  return isVariable(term);
}

bool validFastFuncTerm(FastTerm term)
{
  assert(validFastTerm(term));
  return isFuncTerm(term);
}

bool validFastTerm(FastTerm term)
{
  assert(/* 0 <= termDataSize && */termDataSize < MAXDATA);
  return (validFastVar(term)) ||
    (MAXVARS <= term && term < MAXVARS + termDataSize);
}

FastTerm newFuncTerm(FastFunc func, FastTerm *args)
{
  if (termDataSize + getArity(func) > MAXDATA) {
    fprintf(stderr, "Too much term data.\n");
    exit(-1);
  }
  uint32 result = termDataSize;
  termData[result] = func;
  termDataSize++;
  for (uint i = 0; i < getArity(func); ++i) {
    termData[result + i + 1] = args[i];
    termDataSize++;
  }
  assert(validFastTerm(result + MAXVARS));
  return result + MAXVARS;
}

bool isVariable(FastTerm term)
{
  assert(validFastTerm(term));
  return validFastVar(term);
}

bool isFuncTerm(FastTerm term)
{
  assert(validFastTerm(term));
  return MAXVARS <= term && term < MAXVARS + termDataSize;
}

size_t printTermRec(FastTerm term, char *buffer, size_t size)
{
  if (isVariable(term)) {
    assert(/* 0 <= term && */term < MAXVARS);
    size_t len = strlen(getVarName(term));
    if (len <= size) {
      for (size_t i = 0; i < len; ++i) {
        buffer[i] = getVarName(term)[i];
      }
    }
    return len;
  } else {
    assert(isFuncTerm(term));
    assert(term >= MAXVARS);
    size_t index = term - MAXVARS;
    assert(/* 0 <= index && */ index < termDataSize);

    FastFunc func = termData[index];
    size_t len = strlen(getFuncName(func));
    size_t printedResult = 0;
    if (len <= size) {
      for (size_t i = 0; i < len; ++i) {
        buffer[i] = getFuncName(func)[i];
        printedResult++;
      }

      buffer = buffer + len;
      size = size - len;
      if (size >= 1 && getArity(func) > 0) {
        buffer[0] = '(';
        size--;
        printedResult++;
        buffer++;
      }
      for (uint i = 0; i < getArity(func); ++i) {
        if (size >= 1 && i > 0) {
          buffer[0] = ',';
          size--;
          printedResult++;
          buffer++;
        }
        size_t printed = printTermRec(termData[index + i + 1], buffer, size);
        buffer = buffer + printed;
        size = size - printed;
        printedResult += printed;
      }
      if (size >= 1 && getArity(func) > 0) {
        buffer[0] = ')';
        size--;
        printedResult++;
        buffer++;
      }
    }
    return printedResult;
  }
}

size_t printTerm(FastTerm term, char *buffer, size_t size)
{
  assert(size > 0);
  size_t printed = printTermRec(term, buffer, size);
  if (printed < size) {
    buffer[printed] = 0;
    return printed;
  } else {
    buffer[size - 1] = 0;
    return size - 1;
  }
}

FastFunc getFunc(FastTerm term)
{
  assert(isFuncTerm(term));
  assert(term >= MAXVARS);
  uint32 index = term - MAXVARS;
  return termData[index];
}

FastTerm getArg(FastTerm term, uint arg)
{
  assert(validFastTerm(term));
  assert(validFastFuncTerm(term));
  assert(/* 0 <= arg && */arg < getArity(getFunc(term)));
  return termData[term - MAXVARS + arg + 1];
}

FastTerm *args(FastTerm term)
{
  assert(isFuncTerm(term));
  assert(term >= MAXVARS);
  uint32 index = term - MAXVARS;
  return &termData[index + 1];
}

bool occurs(FastVar var, FastTerm term)
{
  if (term == var) {
    return true;
  }
  if (isFuncTerm(term)) {
    FastFunc func = getFunc(term);
    for (uint i = 0; i < getArity(func); ++i) {
      if (occurs(var, getArg(term, i))) {
        return true;
      }
    }
    return false;
  }
  return false;
}

bool identifierTaken(const char *name)
{
  return existsSort(name) || existsVar(name) || existsFunc(name);
}

FastSort getSort(FastTerm term)
{
  assert(validFastTerm(term));
  if (isVariable(term)) {
    return getVarSort(term);
  } else {
    assert(validFastFuncTerm(term));
    return getFuncSort(getFunc(term));
  }
}

bool eq_term_list(FastTerm *tl1, FastTerm *tl2, uint count)
{
  for (uint i = 0; i < count; ++i) {
    if (!eq_term(tl1[i], tl2[i])) {
      return false;
    }
  }
  return true;
}

char buffer[1024];

bool eq_term(FastTerm t1, FastTerm t2)
{
  // printTerm(t1, buffer, 1024);
  // printf("eq1 %s (%d)\n", buffer, t1);
  // printTerm(t1, buffer, 1024);
  // printf("eq2 %s (%d)\n", buffer, t2);
  assert(validFastTerm(t1));
  assert(validFastTerm(t2));
  if (isFuncTerm(t1) && isFuncTerm(t2)) {
    FastFunc func1 = getFunc(t1);
    FastFunc func2 = getFunc(t2);
    if (!eq_func(func1, func2)) {
      return false;
    }
    FastTerm *args1 = args(t1);
    FastTerm *args2 = args(t2);
    assert(getArity(func1) == getArity(func2));
    return eq_term_list(args1, args2, getArity(func1));
  } else if (isVariable(t1) && isVariable(t2)) {
    return eq_var(t1, t2);
  } else {
    return false;
  }
}

FastTerm simplifyFast(FastTerm term)
{
  // printTerm(term, buffer, 1024);
  // printf("simplifyFast %s (%d)\n", buffer, term);
  if (isVariable(term)) {
    return term;
  } else {
    assert(isFuncTerm(term));
    FastFunc func = getFunc(term);
    std::vector<FastTerm> arguments;
    for (uint i = 0; i < getArity(func); ++i) {
      arguments.push_back(simplifyFast(getArg(term, i)));
      // printTerm(getArg(term, i), buffer, 1024);
      // printf("1. inner simplifyFast %s (%d)\n", buffer, getArg(term, i));
      // printTerm(arguments[i], buffer, 1024);
      // printf("2. inner simplifyFast %s (%d)\n", buffer, arguments[i]);
    }
    FastTerm result = newFuncTerm(func, &arguments[0]);
    FastTerm t1, t2;
    if (isBuiltinFunc(func)) {
      //      printf("is builtin\n");
      if (getArity(func) >= 1) {
	t1 = arguments[0];
      }
      if (getArity(func) >= 2) {
       t2 = arguments[1];
      }
      switch (getBuiltinFuncType(func)) {
      case bltnAnd:
	if (eq_term(t1, fastFalse())) {
	  return fastFalse();
	}
	if (eq_term(t2, fastFalse())) {
	  return fastFalse();
	}
	if (eq_term(t1, fastTrue())) {
	  return t2;
	}
	if (eq_term(t2, fastTrue())) {
	  return t1;
	}
	if (eq_term(t1, t2)) {
	  return t1;
	}
	break;
      case bltnOr:
	if (eq_term(t1, fastTrue())) {
	  return fastTrue();
	}
	if (eq_term(t2, fastTrue())) {
	  return fastTrue();
	}
	if (eq_term(t1, fastFalse())) {
	  return t2;
	}
	if (eq_term(t2, fastFalse())) {
	  return t1;
	}
	if (eq_term(t1, t2)) {
	  return t1;
	}
	break;
      case bltnNot:
	if (eq_term(t1, fastFalse())) {
	  return fastTrue();
	}
	if (eq_term(t1, fastTrue())) {
	  return fastFalse();
	}
	break;
      case bltnImplies:
	if (eq_term(t1, fastFalse())) {
	  return fastTrue();
	}
	if (eq_term(t2, fastFalse())) {
	  return t1;
	}
	if (eq_term(t1, fastTrue())) {
	  return t2;
	}
	if (eq_term(t2, fastTrue())) {
	  return fastTrue();
	}
	if (eq_term(t1, t2)) {
	  return fastTrue();
	}
	break;
      case bltnEqInt:
	if (eq_term(t1, t2)) {
	  return fastTrue();
	}
	break;
      case bltnEqArray:
	if (eq_term(t1, t2)) {
	  return fastTrue();
	}
	break;
      case bltnSelect:
	if (isFuncTerm(arguments[0]) &&
	    isBuiltinFunc(getFunc(arguments[0])) &&
	    getBuiltinFuncType(getFunc(arguments[0])) == bltnStore) {
	  FastTerm *argsstore = args(arguments[0]);
	  assert(getArity(getFunc(arguments[0])) == 3);
	  if (eq_term(arguments[1], argsstore[1])) {
	    return argsstore[2];
	  }
	}
	return result;
	break;
      case bltnStore:
	if (isFuncTerm(arguments[0]) &&
	    isBuiltinFunc(getFunc(arguments[0])) &&
	    getBuiltinFuncType(getFunc(arguments[0])) == bltnStore) {
	  FastTerm *argsstore = args(arguments[0]);
	  assert(getArity(getFunc(arguments[0])) == 3);
	  assert(validFastTerm(arguments[1]));
	  assert(validFastTerm(argsstore[1]));
	  if (eq_term(arguments[1], argsstore[1])) {
	    FastTerm newargs[4];
	    newargs[0] = argsstore[0];
	    newargs[1] = argsstore[1];
	    newargs[2] = arguments[2];
	    return newFuncTerm(func, newargs);
	  }
	}
	return result;
	break;
      case bltnEqBool:
	if (eq_term(t1, t2)) {
	  return fastTrue();
	}
	if (eq_term(t1, fastTrue())) {
	  return t2;
	}
	if (eq_term(t2, fastTrue())) {
	  return t1;
	}
	break;
      default:
	break;
      }
    }
    return result;
  }
}

FastTerm replaceConstWithVar(FastTerm term, FastTerm c, FastVar v)
{
  if (isVariable(term)) {
    return term;
  } else if (eq_term(term, c)) {
    return v;
  } else {
    bool changed = false;
    FastFunc func = getFunc(term);
    uint32 count = getArity(func);
    std::vector<FastTerm> newargs;
    for (uint i = 0; i < count; ++i) {
      FastTerm result = replaceConstWithVar(getArg(term, i), c, v);
      if (result != getArg(term, i)) {
	changed = true;
      }
      newargs.push_back(result);
    }
    if (changed) {
      return newFuncTerm(func, &newargs[0]);
    } else {
      return term;
    }
  }
}

std::string toString(FastTerm term)
{
  ostringstream oss;
  printToOss(term, oss);
  return oss.str();
}

std::string toString(FastSubst subst)
{
  ostringstream oss;
  oss << "{ ";
  for (uint i = 0; i < subst.count; i += 2) {
    if (i > 0) {
      oss << ", ";
    }
    assert(validFastTerm(subst.data[i + 1]));
    assert(isVariable(subst.data[i]));
    printToOss(subst.data[i], oss);
    oss << " |-> ";
    assert(validFastTerm(subst.data[i + 1]));
    printToOss(subst.data[i + 1], oss);
  }
  oss << " }";
  return oss.str();
}

void printToOss(FastTerm term, ostringstream &oss)
{
  if (isVariable(term)) {
    assert(/* 0 <= term && */term < MAXVARS);
    oss << getVarName(term);
  } else {
    assert(isFuncTerm(term));
    assert(term >= MAXVARS);
    size_t index = term - MAXVARS;
    assert(/* 0 <= index && */ index < termDataSize);

    FastFunc func = termData[index]; 
    if (getArity(func) > 0) {
      oss << "(";
    }
    oss << getFuncName(func);
    for (uint i = 0; i < getArity(func); ++i) {
      oss << " ";
      printToOss(termData[index + i + 1], oss);
    }
    if (getArity(func) > 0) {
      oss << ")";
    }
  }
}

#include <iostream>

void abortWithMessage(const std::string &error)
{
  std::cout << "Error: " << error << endl;
  exit(0);
}

void varsOf(FastTerm term, vector<FastVar> &vars)
{
  if (isVariable(term)) {
    vars.push_back(term);
  } else {
    assert(isFuncTerm(term));
    FastFunc func = getFunc(term);
    for (uint i = 0; i < getArity(func); ++i) {
      FastTerm nt = getArg(term, i);
      varsOf(nt, vars);
    }
  }
}

vector<FastVar> uniqueVars(FastTerm term)
{
  vector<FastVar> result;
  varsOf(term, result);
  std::sort(result.begin(), result.end());
  auto it = std::unique(result.begin(), result.end());
  result.resize(std::distance(result.begin(), it));
  return result;
}
