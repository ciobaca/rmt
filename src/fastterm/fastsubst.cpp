#include "fastterm.h"
#include <cassert>

FastSubst newSubst()
{
  FastSubst subst = (FastSubst)malloc(sizeof(*subst) + 16 * sizeof(uint32));
  subst->size = 16;
  subst->count = 0;
  return subst;
}

FastSubst addToSubst(FastSubst subst, FastVar var, FastTerm term)
{
  if (subst->count < subst->size) {
    subst->data[subst->count++] = var;
    subst->data[subst->count++] = term;
    return subst;
  } else {
    assert(0);
    return subst;
    // TODO: realloc struct, return new subst
  }
}

bool inDomain(FastVar var, FastSubst subst)
{
  for (uint i = 0; i < subst->count; i += 2) {
    if (subst->data[i] == var) {
      return true;
    }
  }
  return false;
}

FastTerm range(FastVar var, FastSubst subst)
{
  assert(inDomain(var, subst));
  for (uint i = 0; i < subst->count; i++) {
    if (subst->data[i] == var) {
      return subst->data[i + 1];
    }
  }
  assert(0);
  return 0;
}

FastTerm applySubst(FastTerm term, FastSubst subst)
{
  if (isVariable(term)) {
    if (inDomain(term, subst)) {
      return range(term, subst);
    }
    return term;
  } else {
    assert(isFuncTerm(term));
    FastFunc func = getFunc(term);
    FastTerm result = newFuncTerm(func, args(term));
    FastTerm *arguments = args(result);
    for (uint i = 0; i < getArity(func); ++i) {
      *arguments = applySubst(*arguments, subst);
      arguments++;
    }
    return result;
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

FastSubst composeSubst(FastSubst subst, FastVar v, FastTerm t)
{
  bool found = false;
  for (uint i = 0; i < subst->count; i += 2) {
    FastVar x = subst->data[i];
    FastTerm s = subst->data[i + 1];
    if (x == v) {
      found = true;
    }
    subst->data[i + 1] = applyUnitySubst(s, v, t);
  }
  if (!found) {
    return addToSubst(subst, v, t);
  } else {
    return subst;
  }
}

size_t printSubst(FastSubst subst, char *buffer, size_t size)
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
  for (uint i = 0; i < subst->count; i += 2) {
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
    assert(validFastTerm(subst->data[i + 1]));
    assert(isVariable(subst->data[i]));
    uint32 printed = printTerm(subst->data[i], buffer, size);
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
    assert(validFastTerm(subst->data[i + 1]));
    printed = printTerm(subst->data[i], buffer, size);
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

