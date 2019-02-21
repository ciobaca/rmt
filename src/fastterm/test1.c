#include "fastterm.h"
#include <stdio.h>

int main()
{
  char buffer[1024];

  FastSort sort = newSort("State");
  FastVar x1 = newVar("x1", sort);
  FastVar x2 = newVar("x2", sort);
  /* FastVar x3 = newVar("x3", sort); */
  FastFunc e = newConst("e", sort);

  FastSort sorts[16];
  for (int i = 0; i < 16; ++i) {
    sorts[i] = sort;
  }

  FastFunc i = newFunc("i", sort, 1, sorts);
  FastFunc f = newFunc("f", sort, 2, sorts);
  FastFunc g = newFunc("g", sort, 3, sorts);

  FastTerm array[16];

  array[0] = x1;
  FastTerm t1 = newFuncTerm(i, array);
  printTerm(t1, buffer, 1024);
  printf("t1 = %s\n", buffer);

  array[0] = x1;
  array[1] = x2;
  FastTerm t2 = newFuncTerm(f, array);
  printTerm(t2, buffer, 1024);
  printf("t2 = %s\n", buffer);

  array[0] = t1;
  array[1] = t2;
  FastTerm t3 = newFuncTerm(f, array);
  printTerm(t3, buffer, 1024);
  printf("t3 = %s\n", buffer);

  FastTerm t4 = newFuncTerm(e, array);
  printTerm(t4, buffer, 1024);
  printf("t4 = %s\n", buffer);

  array[0] = t1;
  array[1] = t2;
  array[2] = t4;
  FastTerm t5 = newFuncTerm(g, array);
  printTerm(t5, buffer, 1024);
  printf("t5 = %s\n", buffer);
}
