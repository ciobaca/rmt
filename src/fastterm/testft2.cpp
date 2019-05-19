#include "fastterm.h"
#include <stdio.h>

void test1()
{
  char buffer[1024];

  FastSort sort = newSort("State");
  FastVar x = newVar("x", sort);
  FastVar x1 = newVar("x1", sort);
  FastVar x2 = newVar("x2", sort);

  FastSort sorts[16];
  for (int i = 0; i < 16; ++i) {
    sorts[i] = sort;
  }

  FastFunc f = newFunc("f", sort, 2, sorts);
  FastFunc g = newFunc("g", sort, 1, sorts);

  FastTerm array[16];

  array[0] = x;
  array[1] = x;
  FastTerm t1 = newFuncTerm(f, array);
  printTerm(t1, buffer, 1024);
  printf("t1 = %s.\n", buffer);

  array[0] = x1;
  FastTerm t2a = newFuncTerm(g, array);
  printTerm(t2a, buffer, 1024);
  printf("t2a = %s.\n", buffer);

  array[0] = x2;
  FastTerm t2b = newFuncTerm(g, array);
  printTerm(t2b, buffer, 1024);
  printf("t2b = %s.\n", buffer);

  array[0] = t2a;
  array[1] = t2b;
  FastTerm t2 = newFuncTerm(f, array);
  printTerm(t2, buffer, 1024);
  printf("t2 = %s.\n", buffer);

  FastSubst subst;
  if (unify(t1, t2, &subst)) {
    printf("Unification successful.\n");
    printSubst(subst, buffer, 1024);
    printf("unifier = %s.\n", buffer);
  } else {
    printf("Unification failed.\n");
  }
}

void test2()
{
  char buffer[1024];

  FastSort sort = newSort("State");
  FastVar x = newVar("x", sort);
  //  FastVar x1 = newVar("x1", sort);
  FastVar x2 = newVar("x2", sort);

  FastSort sorts[16];
  for (int i = 0; i < 16; ++i) {
    sorts[i] = sort;
  }

  FastFunc f = newFunc("f", sort, 2, sorts);
  FastFunc g = newFunc("g", sort, 1, sorts);

  FastTerm array[16];

  array[0] = x;
  array[1] = x;
  FastTerm t1 = newFuncTerm(f, array);
  printTerm(t1, buffer, 1024);
  printf("t1 = %s.\n", buffer);

  array[0] = x;
  FastTerm t2a = newFuncTerm(g, array);
  printTerm(t2a, buffer, 1024);
  printf("t2a = %s.\n", buffer);

  array[0] = x2;
  FastTerm t2b = newFuncTerm(g, array);
  printTerm(t2b, buffer, 1024);
  printf("t2b = %s.\n", buffer);

  array[0] = t2a;
  array[1] = t2b;
  FastTerm t2 = newFuncTerm(f, array);
  printTerm(t2, buffer, 1024);
  printf("t2 = %s.\n", buffer);

  FastSubst subst;
  if (unify(t1, t2, &subst)) {
    printf("Unification successful.\n");
    printSubst(subst, buffer, 1024);
    printf("unifier = %s.\n", buffer);
  } else {
    printf("Unification failed.\n");
  }
}

void test3()
{
  char buffer[1024];

  FastSort sort = newSort("State");
  FastVar x = newVar("x", sort);
  FastVar x1 = newVar("x1", sort);
  /* FastVar x2 = newVar("x2", sort); */

  FastSort sorts[16];
  for (int i = 0; i < 16; ++i) {
    sorts[i] = sort;
  }

  FastFunc f = newFunc("f", sort, 2, sorts);
  FastFunc g = newFunc("g", sort, 1, sorts);

  FastTerm array[16];

  array[0] = x;
  array[1] = x;
  FastTerm t1 = newFuncTerm(f, array);
  printTerm(t1, buffer, 1024);
  printf("t1 = %s.\n", buffer);

  array[0] = x1;
  FastTerm t2a = newFuncTerm(g, array);
  printTerm(t2a, buffer, 1024);
  printf("t2a = %s.\n", buffer);

  /* array[0] = x2; */
  /* FastTerm t2b = newFuncTerm(g, array); */
  /* printTerm(t2b, buffer, 1024); */
  /* printf("t2b = %s.\n", buffer); */

  array[0] = t2a;
  array[1] = x1;
  FastTerm t2 = newFuncTerm(f, array);
  printTerm(t2, buffer, 1024);
  printf("t2 = %s.\n", buffer);

  FastSubst subst;
  if (unify(t1, t2, &subst)) {
    printf("Unification successful.\n");
    printSubst(subst, buffer, 1024);
    printf("unifier = %s.\n", buffer);
  } else {
    printf("Unification failed.\n");
  }
}

void test4()
{
  char buffer[1024];

  FastSort sort = newSort("State");
  FastVar x = newVar("x", sort);
  FastVar x1 = newVar("x1", sort);
  /* FastVar x2 = newVar("x2", sort); */

  FastSort sorts[16];
  for (int i = 0; i < 16; ++i) {
    sorts[i] = sort;
  }

  FastFunc f = newFunc("f", sort, 2, sorts);
  FastFunc g = newFunc("g", sort, 1, sorts);
  FastFunc h = newFunc("h", sort, 1, sorts);

  FastTerm array[16];

  array[0] = x;
  array[1] = x;
  FastTerm t1 = newFuncTerm(f, array);
  printTerm(t1, buffer, 1024);
  printf("t1 = %s.\n", buffer);

  array[0] = x1;
  FastTerm t2a = newFuncTerm(g, array);
  printTerm(t2a, buffer, 1024);
  printf("t2a = %s.\n", buffer);

  array[0] = x1;
  FastTerm t2b = newFuncTerm(h, array);
  printTerm(t2b, buffer, 1024);
  printf("t2b = %s.\n", buffer);

  array[0] = t2a;
  array[1] = t2b;
  FastTerm t2 = newFuncTerm(f, array);
  printTerm(t2, buffer, 1024);
  printf("t2 = %s.\n", buffer);

  FastSubst subst;
  if (unify(t1, t2, &subst)) {
    printf("Unification successful.\n");
    printSubst(subst, buffer, 1024);
    printf("unifier = %s.\n", buffer);
  } else {
    printf("Unification failed.\n");
  }
}

void test5()
{
  char buffer[1024];

  FastSort sort = newSort("State");
  FastVar x = newVar("x", sort);
  FastVar x1 = newVar("x1", sort);
  FastVar x2 = newVar("x2", sort);

  FastSort sorts[16];
  for (int i = 0; i < 16; ++i) {
    sorts[i] = sort;
  }

  FastFunc f = newFunc("f", sort, 2, sorts);
  FastFunc g = newFunc("g", sort, 1, sorts);
  FastFunc h = newFunc("h", sort, 1, sorts);

  FastTerm array[16];

  array[0] = x;
  array[1] = x2;
  FastTerm t1 = newFuncTerm(f, array);
  printTerm(t1, buffer, 1024);
  printf("t1 = %s.\n", buffer);

  array[0] = x1;
  FastTerm t2a = newFuncTerm(g, array);
  printTerm(t2a, buffer, 1024);
  printf("t2a = %s.\n", buffer);

  array[0] = x1;
  FastTerm t2b = newFuncTerm(h, array);
  printTerm(t2b, buffer, 1024);
  printf("t2b = %s.\n", buffer);

  array[0] = t2a;
  array[1] = t2b;
  FastTerm t2 = newFuncTerm(f, array);
  printTerm(t2, buffer, 1024);
  printf("t2 = %s.\n", buffer);

  FastSubst subst;
  if (unify(t1, t2, &subst)) {
    printf("Unification successful.\n");
    printSubst(subst, buffer, 1024);
    printf("unifier = %s.\n", buffer);
  } else {
    printf("Unification failed.\n");
  }
}

int main()
{
  test1();
  test2();
  test3();
  test4();
  test5();
}
