#ifndef DEFINED_H__
#define DEFINED_H__

#include "fastterm.h"
#include "rewritesystem.h"

bool isDefinedFunction(FastFunc func);
void makeFunctionDefined(FastFunc func, const RewriteSystem &rs);

FastTerm compute(FastTerm term, Z3_context context);

#endif
