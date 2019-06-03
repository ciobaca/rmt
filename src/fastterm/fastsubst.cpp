#include "fastterm.h"
#include <cassert>
#include <cstring>

FastSubst::FastSubst()
{
  data = (uint *)malloc(sizeof(uint) * 16);// new uint [16];
  // data = new uint [2];
  size = 16;
  count = 0;
}

FastSubst::~FastSubst()
{
  free(data);
  //delete []data;
}

FastSubst::FastSubst(const FastSubst &other) {
  size = other.size;
  count = other.count;
  data = (uint *)malloc(sizeof(uint) * size);
  memcpy(data, other.data, count * sizeof(uint));
}

FastSubst& FastSubst::operator=(const FastSubst &other) {
  size = other.size;
  count = other.count;
  data = (uint *)malloc(sizeof(uint) * size);
  memcpy(data, other.data, count * sizeof(uint));
  return *this;
}

void FastSubst::addToSubst(FastVar var, FastTerm term)
{
  if (count >= size) {
    uint *newdata = (uint *)realloc(data, sizeof(uint) * size * 2);
    //uint *newdata = new uint32 [size * 2];
    size = size * 2;
    //    memmove(newdata, data, count * sizeof(uint));
    // for (uint i = 0; i < count; ++i) {
    //   newdata[i] = data[i];
    // }
    
    //    delete [] data;
    // free(data);
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

FastTerm FastSubst::image(FastVar var)
{
  assert(inDomain(var));
  for (uint i = 0; i < count; i += 2) {
    if (data[i] == var) {
      return data[i + 1];
    }
  }
  assert(0);
  return 0;
}

void FastSubst::replaceConstWithVar(FastTerm c, FastVar v) {
  for (uint i = 0; i < count; ++i) {
    if (data[i] == c) {
      data[i] = v;
    }
  }
}

FastTerm FastSubst::applySubst(FastTerm term)
{
  if (isVariable(term)) {
    if (inDomain(term)) {
      return image(term);
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
    result += printed;
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
    printed = printTerm(subst.data[i + 1], buffer, size);
    buffer += printed;
    size -= printed;
    result += printed;
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

FastSubst1::FastSubst1()
{
  data.reserve(16);
}

FastSubst1::~FastSubst1()
{
}

void FastSubst1::addToSubst1(FastVar var, FastTerm term)
{
  data.push_back(var);
  data.push_back(term);
}

bool FastSubst1::inDomain1(FastVar var)
{
  for (uint i = 0; i < data.size(); i += 2) {
    if (data[i] == var) {
      return true;
    }
  }
  return false;
}

FastTerm FastSubst1::range1(FastVar var)
{
  assert(inDomain(var));
  for (uint i = 0; i < data.size(); i += 2) {
    if (data[i] == var) {
      return data[i + 1];
    }
  }
  assert(0);
  return 0;
}

FastTerm FastSubst1::applySubst1(FastTerm term)
{
  if (isVariable(term)) {
    if (inDomain1(term)) {
      return range1(term);
    }
    return term;
  } else {
    assert(isFuncTerm(term));
    FastFunc func = getFunc(term);
    FastTerm result = newFuncTerm(func, args(term));
    FastTerm *arguments = args(result);
    for (uint i = 0; i < getArity(func); ++i) {
      *arguments = this->applySubst1(*arguments);
      arguments++;
    }
    return result;
  }
}

void FastSubst1::composeWith1(FastVar v, FastTerm t)
{
  bool found = false;
  for (uint i = 0; i < data.size(); i += 2) {
    FastVar x = data[i];
    FastTerm s = data[i + 1];
    if (x == v) {
      found = true;
    }
    data[i + 1] = applyUnitySubst(s, v, t);
  }
  if (!found) {
    addToSubst1(v, t);
  }
}

size_t printSubst1(FastSubst1 &subst, char *buffer, size_t size)
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
  for (uint i = 0; i < subst.data.size(); i += 2) {
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


FastSubst2::FastSubst2()
{
  reserve(16);
}

FastSubst2::~FastSubst2()
{
}

void FastSubst2::addToSubst2(FastVar var, FastTerm term)
{
  push_back(var);
  push_back(term);
}

bool FastSubst2::inDomain2(FastVar var)
{
  for (uint i = 0; i < size(); i += 2) {
    if ((*this)[i] == var) {
      return true;
    }
  }
  return false;
}

FastTerm FastSubst2::range2(FastVar var)
{
  assert(inDomain2(var));
  for (uint i = 0; i < size(); i += 2) {
    if ((*this)[i] == var) {
      return (*this)[i + 1];
    }
  }
  assert(0);
  return 0;
}

FastTerm FastSubst2::applySubst2(FastTerm term)
{
  if (isVariable(term)) {
    if (inDomain2(term)) {
      return range2(term);
    }
    return term;
  } else {
    assert(isFuncTerm(term));
    FastFunc func = getFunc(term);
    FastTerm result = newFuncTerm(func, args(term));
    FastTerm *arguments = args(result);
    for (uint i = 0; i < getArity(func); ++i) {
      *arguments = this->applySubst2(*arguments);
      arguments++;
    }
    return result;
  }
}

void FastSubst2::composeWith2(FastVar v, FastTerm t)
{
  bool found = false;
  for (uint i = 0; i < size(); i += 2) {
    FastVar x = (*this)[i];
    FastTerm s = (*this)[i + 1];
    if (x == v) {
      found = true;
    }
    (*this)[i + 1] = applyUnitySubst(s, v, t);
  }
  if (!found) {
    addToSubst2(v, t);
  }
}

size_t printSubst2(FastSubst2 &subst, char *buffer, size_t size)
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
  for (uint i = 0; i < subst.size(); i += 2) {
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
    assert(validFastTerm(subst[i + 1]));
    assert(isVariable(subst[i]));
    uint32 printed = printTerm(subst[i], buffer, size);
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
    assert(validFastTerm(subst[i + 1]));
    printed = printTerm(subst[i], buffer, size);
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
