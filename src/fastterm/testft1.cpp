#include "fastterm.h"
#include "fastqueryacunify.h"
#include "unifeqsystem.h"
#include <stdio.h>
#include <z3.h>

int test_fastterm()
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

  return 0;
}

// int test_z3()
// {
//   Z3_context z3context1 = init_z3_context();
//   Z3_context z3context2 = init_z3_context();

//   Z3_sort sort1 = Z3_mk_bool_sort(z3context1);
//   Z3_sort sort3 = Z3_mk_bool_sort(z3context2);
//   Z3_sort sort5 = Z3_mk_int_sort(z3context1);
//   Z3_sort sort7 = Z3_mk_int_sort(z3context2);

//   printf("Sorts: %p %p %p %p\n", sort1, sort3, sort5, sort7);

//   Z3_symbol symb1 = Z3_mk_string_symbol(z3context1, "symb1");
//   Z3_symbol symb2 = Z3_mk_string_symbol(z3context1, "symb2");
//   Z3_symbol symb3 = Z3_mk_string_symbol(z3context1, "symb1");
//   Z3_symbol symb4 = Z3_mk_string_symbol(z3context1, "symb2");
  
//   printf("Symbs 1: %p %p %p %p\n", symb1, symb2, symb3, symb4);
  
//   Z3_symbol symb5 = Z3_mk_string_symbol(z3context2, "symb1");
//   Z3_symbol symb6 = Z3_mk_string_symbol(z3context2, "symb2");
//   Z3_symbol symb7 = Z3_mk_string_symbol(z3context2, "symb1");
//   Z3_symbol symb8 = Z3_mk_string_symbol(z3context2, "symb2");
  
//   printf("Symbs 2: %p %p %p %p\n", symb5, symb6, symb7, symb8);

//   Z3_ast const1 = Z3_mk_const(z3context1, symb1, sort1);
//   Z3_ast const2 = Z3_mk_const(z3context1, symb1, sort5);
  
//   printf("Consts 1: %p %p\n", const1, const2);
  
//   Z3_ast const3 = Z3_mk_const(z3context2, symb1, sort1);
//   Z3_ast const4 = Z3_mk_const(z3context2, symb1, sort5);
  
//   printf("Consts 2: %p %p\n", const3, const4);
  
//   Z3_ast const5 = Z3_mk_const(z3context2, symb1, sort3);
//   Z3_ast const6 = Z3_mk_const(z3context2, symb1, sort7);
  
//   printf("Consts 3: %p %p\n", const5, const6);

//   Z3_ast const7 = Z3_mk_const(z3context2, symb1, sort3);
//   Z3_ast const8 = Z3_mk_const(z3context2, symb1, sort7);
  
//   printf("Consts 4: %p %p\n", const7, const8);
  
//   return 0;
// }

int test_builtins()
{
  char buffer[1024];

  FastSort boolSort = fastBoolSort();
  FastVar bx1 = newVar("bx1", boolSort);
  FastVar bx2 = newVar("bx2", boolSort);
  FastTerm bt1 = fastNot(bx2);
  FastTerm bt2 = fastAnd(bx1, bt1);
  FastTerm bt3 = fastAnd(bx2, bt1);

  printTerm(bt2, buffer, 1024);
  printf("0. term = %s\n", buffer);

  Z3_context context = init_z3_context();
  
  //  printf("bt2 (z3) = %s\n", Z3_ast_to_string(context, toZ3Term(context, bt2)));
  printf("1. sat = %d\n", z3_sat_check(context, bt2));

  printf("2. sat = %d\n", z3_sat_check(context, bt3));

  Z3_solver solver = init_z3_solver(context);

  z3_assert(context, solver, bt2);

  printf("3. sat = %d.\n", z3_check(context, solver));

  z3_push(context, solver);
  //  printf("Creating undo point.\n");

  z3_assert(context, solver, bx2);
  //  printf("Asserting bx2 = %s.\n", Z3_ast_to_string(context, toZ3Term(context, bx2)));

  printf("4. sat = %d.\n", z3_check(context, solver));

  z3_pop(context, solver);

  printf("5. sat = %d.\n", z3_check(context, solver));

  return 0;
}

void test_unify()
{
  FastSort sort = newSort("State");
  FastVar x1 = newVar("x1", sort);
  FastVar x2 = newVar("x2", sort);
  FastVar x3 = newVar("x3", sort);
  FastVar x4 = newVar("x4", sort);
  FastFunc e = newConst("e", sort);

  FastSort sorts[16];
  for (int i = 0; i < 16; ++i) {
    sorts[i] = sort;
  }

  FastFunc i = newFunc("i", sort, 1, sorts);
  FastFunc f = newFunc("f", sort, 2, sorts);
  //  FastFunc g = newFunc("g", sort, 3, sorts);

  FastTerm array[16];

  array[0] = x1;
  FastTerm t1 = newFuncTerm(i, array);

  array[0] = x1;
  array[1] = x2;
  FastTerm t2 = newFuncTerm(f, array);

  array[0] = x3;
  array[1] = x4;
  FastTerm t3 = newFuncTerm(f, array);

  FastTerm t4 = newFuncTerm(e, array);

  array[0] = x3;
  array[1] = t4;
  FastTerm t5 = newFuncTerm(f, array);
 
  array[0] = x3;
  array[1] = x3;
  FastTerm t6 = newFuncTerm(f, array);

  array[0] = t4;
  array[1] = t1;
  FastTerm t7 = newFuncTerm(f, array);

  // array[0] = t1;
  // array[1] = t2;
  // array[2] = t4;
  // FastTerm t10 = newFuncTerm(g, array);

  char buffer1[1024];
  char buffer2[1024];
  char buffer3[1024];
  char buffer4[1024];
  char buffer5[1024];
  char buffer6[1024];
  char buffer7[1024];
  printTerm(t1, buffer1, 1024);
  printTerm(t2, buffer2, 1024);
  printTerm(t3, buffer3, 1024);
  printTerm(t4, buffer4, 1024);
  printTerm(t5, buffer5, 1024);
  printTerm(t6, buffer6, 1024);
  printTerm(t7, buffer7, 1024);

  FastSubst subst1;
  printf("unify %s and %s: %d\n", buffer1, buffer2, unify(t1, t2).size());
  
  FastSubst subst2;
  printf("unify %s and %s: %d\n", buffer2, buffer3, unify(t2, t3).size());
  
  FastSubst subst3;
  printf("unify %s and %s: %d\n", buffer3, buffer5, unify(t3, t5).size());

  FastSubst subst4;
  printf("unify %s and %s: %d\n", buffer5, buffer6, unify(t5, t6).size());

  FastSubst subst5;
  printf("unify %s and %s: %d\n", buffer6, buffer7, unify(t6, t7).size());
}

void test_match()
{
  FastSort sort = newSort("State");
  FastVar x1 = newVar("x1", sort);
  FastVar x2 = newVar("x2", sort);
  FastVar x3 = newVar("x3", sort);
  FastVar x4 = newVar("x4", sort);
  FastFunc e = newConst("e", sort);

  FastSort sorts[16];
  for (int i = 0; i < 16; ++i) {
    sorts[i] = sort;
  }

  FastFunc i = newFunc("i", sort, 1, sorts);
  FastFunc f = newFunc("f", sort, 2, sorts);
  //  FastFunc g = newFunc("g", sort, 3, sorts);

  FastTerm array[16];

  array[0] = x1;
  FastTerm t1 = newFuncTerm(i, array);

  array[0] = x1;
  array[1] = x2;
  FastTerm t2 = newFuncTerm(f, array);

  array[0] = x3;
  array[1] = x4;
  FastTerm t3 = newFuncTerm(f, array);

  FastTerm t4 = newFuncTerm(e, array);

  array[0] = x3;
  array[1] = t4;
  FastTerm t5 = newFuncTerm(f, array);
 
  array[0] = x3;
  array[1] = x3;
  FastTerm t6 = newFuncTerm(f, array);

  array[0] = t4;
  array[1] = t1;
  FastTerm t7 = newFuncTerm(f, array);

  // array[0] = t1;
  // array[1] = t2;
  // array[2] = t4;
  // FastTerm t10 = newFuncTerm(g, array);

  char buffer1[1024];
  char buffer2[1024];
  char buffer3[1024];
  char buffer4[1024];
  char buffer5[1024];
  char buffer6[1024];
  char buffer7[1024];
  printTerm(t1, buffer1, 1024);
  printTerm(t2, buffer2, 1024);
  printTerm(t3, buffer3, 1024);
  printTerm(t4, buffer4, 1024);
  printTerm(t5, buffer5, 1024);
  printTerm(t6, buffer6, 1024);
  printTerm(t7, buffer7, 1024);

  FastSubst subst1;
  printf("match %s and %s: %d\n", buffer1, buffer2, match(t1, t2, subst1).size());

  FastSubst subst2;
  printf("match %s and %s: %d\n", buffer2, buffer3, match(t2, t3, subst2).size());

  FastSubst subst3;
  printf("match %s and %s: %d\n", buffer3, buffer5, match(t3, t5, subst3).size());

  FastSubst subst4;
  printf("match %s and %s: %d\n", buffer5, buffer6, match(t5, t6, subst4).size());

  FastSubst subst5;
  printf("match %s and %s: %d\n", buffer6, buffer7, match(t6, t7, subst5).size());

  FastSubst subst6;
  printf("match %s and %s: %d\n", buffer2, buffer1, match(t2, t1, subst6).size());

  FastSubst subst7;
  printf("match %s and %s: %d\n", buffer3, buffer2, match(t3, t2, subst7).size());

  FastSubst subst8;
  printf("match %s and %s: %d\n", buffer5, buffer3, match(t5, t3, subst8).size());

  FastSubst subst9;
  printf("match %s and %s: %d\n", buffer6, buffer5, match(t6, t5, subst9).size());

  FastSubst subst10;
  printf("match %s and %s: %d\n", buffer7, buffer6, match(t7, t6, subst10).size());
}

int main()
{
  initFastTerm();

  test_fastterm();
  test_builtins();
  test_unify();
  test_match();

  return 0;
}
