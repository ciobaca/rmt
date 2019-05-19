#include "fastterm.h"
#include <cassert>

FastSubst::FastSubst()
{
  data = new uint [16];
  size = 16;
  count = 0;
}

FastSubst::~FastSubst()
{
  delete [] data;
}

void FastSubst::addToSubst(FastVar var, FastTerm term)
{
  if (count >= size) {
    uint32 *newdata = new uint32 [size * 2];
    size = size * 2;
    for (uint i = 0; i < count; ++i) {
      newdata[i] = data[i];
    }
    delete [] data;
    data = newdata;
  }
  data[count++] = var;
  data[count++] = term;
}

bool FastSubst::inDomain(FastVar var)
{
  for (uint i = 0; i < count; i += 2) {
    if (data[i] == var) {
      return true;
    }
  }
  return false;
}

FastTerm FastSubst::range(FastVar var)
{
  assert(inDomain(var));
  for (uint i = 0; i < count; i++) {
    if (data[i] == var) {
      return data[i + 1];
    }
  }
  assert(0);
  return 0;
}

FastTerm FastSubst::applySubst(FastTerm term)
{
  if (isVariable(term)) {
    if (inDomain(term)) {
      return range(term);
    }
    return term;
  } else {
    assert(isFuncTerm(term));
    FastFunc func = getFunc(term);
    FastTerm result = newFuncTerm(func, args(term));
    FastTerm *arguments = args(result);
    for (uint i = 0; i < getArity(func); ++i) {
      *arguments = this->applySubst(*arguments);
      arguments++;
    }
    return result;
  }
}

void FastSubst::composeWith(FastVar v, FastTerm t)
{
  bool found = false;
  for (uint i = 0; i < count; i += 2) {
    FastVar x = data[i];
    FastTerm s = data[i + 1];
    if (x == v) {
      found = true;
    }
    data[i + 1] = applyUnitySubst(s, v, t);
  }
  if (!found) {
    addToSubst(v, t);
  }
}

FastTerm applyUnitySubst(FastTerm term, FastVar v, FastTerm t)
{
  if (isVariable(term)) {
    if (term == v) {
      return t;
    }
    return term;
  } else {
    assert(isFuncTerm(term));
    FastFunc func = getFunc(term);
    FastTerm result = newFuncTerm(func, args(term));
    FastTerm *arguments = args(result);
    for (uint i = 0; i < getArity(func); ++i) {
      *arguments = applyUnitySubst(*arguments, v, t);
      arguments++;
    }
    return result;
  }
}

size_t printSubst(FastSubst &subst, char *buffer, size_t size)
{
  size_t result = 0;
  if (size >= 1) {
    buffer[0] = '{';
    size--;
    result++;
    buffer++;
  }
  if (size >= 1) {
    buffer[0] = ' ';
    size--;
    result++;
    buffer++;
  }
  for (uint i = 0; i < subst.count; i += 2) {
    if (i > 0) {
      if (size >= 1) {
	buffer[0] = ',';
	size--;
	result++;
	buffer++;
      }
      if (size >= 1) {
	buffer[0] = ' ';
	size--;
	result++;
	buffer++;
      }
    }
    assert(validFastTerm(subst.data[i + 1]));
    assert(isVariable(subst.data[i]));
    uint32 printed = printTerm(subst.data[i], buffer, size);
    buffer += printed;
    size -= printed;
    result += size;
    if (size >= 1) {
      buffer[0] = '-';
      size--;
      result++;
      buffer++;
    }
    if (size >= 1) {
      buffer[0] = '>';
      size--;
      result++;
      buffer++;
    }
    assert(validFastTerm(subst.data[i + 1]));
    printed = printTerm(subst.data[i], buffer, size);
    buffer += printed;
    size -= printed;
    result += size;
  }
  if (size >= 1) {
    buffer[0] = ' ';
    size--;
    result++;
    buffer++;
  }
  if (size >= 1) {
    buffer[0] = '}';
    size--;
    result++;
    buffer++;
  }
  return result;
}

