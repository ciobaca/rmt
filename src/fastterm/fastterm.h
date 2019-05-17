#ifndef FASTTERM_H__
#define FASTTERM_H__

#include <stdlib.h>

typedef unsigned int uint32;

#define MAXVARS (1024 * 16)
#define MAXFUNCS 2048
#define MAXDATA (1024 * 1024 * 32)
#define MAXARITYDATA (1024 * 16)
#define MAXSORTS 1024
#define MAXSUBSORTINGS 1024
#define MAXSUBST (1024 * 1024 * 8)

typedef uint32 FastVar;   /* 0 .. MAXVARS - 1 */
typedef uint32 FastFunc;  /* 0 .. MAXFUNCS - 1 */
typedef uint32 FastTerm;  /* 0 .. MAXVARS - 1 and MAXVARS .. */
                          /* if >= MAXVARS, then it represents a pointer into termData */
typedef uint32 FastSort;  /* 0 .. MAXSORTS - 1 */

typedef struct {
  uint32 size;
  uint32 count;
  uint32 data[0];
} FastSubstitutionPrivate;

typedef FastSubstitutionPrivate *FastSubst;

bool validFastVar(FastVar var);
bool validFastFunc(FastFunc func);
bool validFastTerm(FastTerm term);
bool validFastSort(FastSort sort);

FastSort newSort(const char *name);
void newSubSort(FastSort subsort, FastSort supersort);

FastVar newVar(const char *name, FastSort sort);

FastFunc newConst(const char *name, FastSort sort);
FastFunc newFunc(const char *name, FastSort resultSort, uint32 arity, FastSort *args);

FastTerm newFuncTerm(FastFunc func, FastTerm *args);

bool isVariable(FastTerm term);
bool isFuncTerm(FastTerm term);

FastFunc funcSymbol(FastTerm term);

size_t printTerm(FastTerm term, char *buffer, size_t size);

FastSubst newSubst();
void addToSubst(FastVar var, FastTerm term);

bool unify(FastTerm t1, FastTerm t2, FastSubst *result);

size_t printSubst(FastSubst subst, char *buffer, size_t size);

#endif
