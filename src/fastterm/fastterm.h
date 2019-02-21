#ifndef FASTTERM_H__
#define FASTTERM_H__

#include <stdlib.h>

typedef unsigned int uint32;

#define MAXVARS 1024
#define MAXFUNCS 2048
#define MAXDATA (1024 * 1024 * 32)
#define MAXARITYDATA (1024 * 16)
#define MAXSORTS 1024
#define MAXSUBSORTINGS 1024

typedef uint32 FastVar;  /*       0 .. MAXVARS - 1 */
typedef uint32 FastFunc; /*       0 .. MAXFUNCS - 1 */
typedef uint32 FastTerm; /*       0 .. MAXVARS - 1 and MAXVARS .. */
typedef uint32 FastSort; /* 0 .. MAXSORTS - 1 */

/* uint32 sortCount; */
/* char *sortNames[MAXSORTS]; */

/* FastSort subsorts[MAXSUBSORTINGS * 2]; */
/* uint32 subsortCount; */

/* uint32 varCount; */
/* FastSort varSorts[MAXVARS]; */
/* char *varNames[MAXVARS]; */

/* uint32 funcCount; */
/* uint32 arities[MAXFUNCS]; // number of arguments */
/* uint32 aritiesIndex[MAXFUNCS]; // where in arityData the argument sorts are */
/* FastSort resultSorts[MAXFUNCS]; // the result sort */
/* FastSort arityData[MAXARITYDATA]; // at arityData[arityIndex[f]] start the */
/*                                   // arity[f] sorts of the arguments */
/* uint32 arityDataIndex; */
/* char *funcNames[MAXFUNCS]; */

/* uint32 termDataSize; */
/* uint32 termData[MAXDATA]; */

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

size_t printTerm(FastTerm term, char *buffer, size_t size);

#endif
