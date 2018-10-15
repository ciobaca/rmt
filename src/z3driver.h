#ifndef Z3_DRIVER__
#define Z3_DRIVER__

#include <vector>
#include <string>
#include <z3.h>

struct Sort;

struct Variable;
struct Term;

enum Z3Result
{
  sat,
  unsat,
  unknown
};

struct Z3Theory
{
  std::vector<Variable *> variables;
  std::vector<Term *> constraints;
  
  void addVariable(Variable *);
  void addEqualityConstraint(Term *, Term *);
  void addConstraint(Term *);

  Z3Result isSatisfiable();
};

Z3Result isSatisfiable(Term *constraint);

void parse_z3_prelude(std::string);
void start_z3_api();
void test_z3_api();

typedef Z3_ast (*func_interpretation)(std::vector<Term *>);

Z3_sort z3_bool();

Z3_sort z3_int();

Z3_ast z3_add(std::vector<Term *> args);

Z3_ast z3_mul(std::vector<Term *> args);

Z3_ast z3_sub(std::vector<Term *> args);

Z3_ast z3_div(std::vector<Term *> args);

Z3_ast z3_mod(std::vector<Term *> args);

Z3_ast z3_le(std::vector<Term *> args);

Z3_ast z3_lt(std::vector<Term *> args);

Z3_ast z3_eq(std::vector<Term *> args);

Z3_ast z3_ct_0(std::vector<Term *> args);

Z3_ast z3_ct_1(std::vector<Term *> args);

Z3_ast z3_ct_2(std::vector<Term *> args);

Z3_ast z3_ct_3(std::vector<Term *> args);

Z3_ast z3_ct_4(std::vector<Term *> args);

Z3_ast z3_ct_5(std::vector<Term *> args);

Z3_ast z3_ct_6(std::vector<Term *> args);

Z3_ast z3_ct_7(std::vector<Term *> args);

Z3_ast z3_ct_8(std::vector<Term *> args);

Z3_ast z3_ct_9(std::vector<Term *> args);

Z3_ast z3_ct_10(std::vector<Term *> args);

Z3_ast z3_ct_11(std::vector<Term *> args);

Z3_ast z3_ct_12(std::vector<Term *> args);

Z3_ast z3_ct_13(std::vector<Term *> args);

Z3_ast z3_ct_14(std::vector<Term *> args);

Z3_ast z3_ct_15(std::vector<Term *> args);

Z3_ast z3_true(std::vector<Term *> args);

Z3_ast z3_false(std::vector<Term *> args);

Z3_ast z3_not(std::vector<Term *> args);

Z3_ast z3_and(std::vector<Term *> args);

Z3_ast z3_or(std::vector<Term *> args);

Z3_ast z3_iff(std::vector<Term *> args);

Z3_ast z3_implies(std::vector<Term *> args);

Z3_ast z3_make_constant(Sort *);

#endif
