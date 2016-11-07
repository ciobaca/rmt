#include "queryimplies.h"
#include "parse.h"
#include "factories.h"
#include "z3driver.h"
#include <iostream>
#include <string>
#include <map>
#include <cassert>

using namespace std;

QueryImplies::QueryImplies() :
  ct(0, 0)
{
}

Query *QueryImplies::create()
{
  return new QueryImplies();
}
  
void QueryImplies::parse(std::string &s, int &w)
{
  matchString(s, w, "implies");
  skipWhiteSpace(s, w);
  ct = parseConstrainedTerm(s, w);
  skipWhiteSpace(s, w);
  // matchString(s, w, "and");
  // skipWhiteSpace(s, w);
  term = parseTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");
}

void QueryImplies::execute()
{
  Substitution subst;
  Term *constraint = 0;
  cout << "Testing implication between " << ct.toString() << " and " << term->toString() << endl;
  //    cerr << "here" << endl;
  if (term->unifyModuloTheories(ct.term, subst, constraint)) {
    //      cerr << "here2" << endl;
    // Function *TrueFun = getFunction("true");
    // Function *AndFun = getFunction("band");
    // Function *OrFun = getFunction("bor");
    // Function *NotFun = getFunction("bnot");
    // Function *EqualsFun = getFunction("bequals");
    
    // assert(TrueFun);
    // assert(AndFun);
    // assert(OrFun);
    // assert(NotFun);
    // assert(EqualsFun);
    
    // query.ct.constraint -> constraint is valid iff
    // (not query.ct.constraint or constraint) is valid iff
    // not (not query.ct.constraint or constraint) is not satisfiable
    // query.ct.constraint and not constraint is not satisfiable
    //      cerr << "here3" << endl;
    if (ct.constraint == 0) {
      vector<Term *> empty;
      ct.constraint = bTrue();
    }
    Term *constraintToCheck = bAnd(ct.constraint, bNot(constraint));
    //      cerr << "here4" << endl;
    Z3Result result = isSatisfiable(constraintToCheck);
    //      cerr << "here5" << endl;
    
    cout << "Implication results:" << endl;
    if (result == sat) {
      cout << "The implication is not valid" << endl;
    } else if (result == unsat) {
      cout << "The implication is valid" << endl;
    } else if (result == unknown) {
      cout << "Implication validity check inconclusive." << endl;
    }
  } else {
    cout << "No implication" << endl;
  }
}
