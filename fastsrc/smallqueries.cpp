#include "smallqueries.h"
#include "fastterm.h"
#include "parse.h"
#include "abstract.h"
#include "helper.h"
#include "log.h"
#include "search.h"
#include "smtunify.h"
#include "rewritesystem.h"
#include <iostream>
#include <map>
#include <vector>

using namespace std;

extern map<string, RewriteSystem> rewriteSystems;

void processAbstract(string &s, int &w)
{
  skipWhiteSpace(s, w);
  matchString(s, w, "abstract");
  skipWhiteSpace(s, w);
  FastTerm term = parseTerm(s, w);
  skipWhiteSpace(s, w);
  match(s, w, ';');
  skipWhiteSpace(s, w);

  FastTerm resultTerm;
  FastSubst resultSubst;
  abstractTerm(term, resultTerm, resultSubst);
  cout << "Abstracting          " << toString(term) << ":" << endl;
  cout << "Abstract term     =  " << toString(resultTerm) << endl;
  cout << "Abstracting subst =  " << toString(resultSubst) << endl;
}

void processSearch(string &s, int &w)
{
  std::string rewriteSystemName;

  int minDepth;
  int maxDepth;

  matchString(s, w, "search");
  skipWhiteSpace(s, w);
  if (lookAhead(s, w, "[")) {
    matchString(s, w, "[");
    skipWhiteSpace(s, w);
    minDepth = getNumber(s, w);
    skipWhiteSpace(s, w);
    matchString(s, w, ",");
    skipWhiteSpace(s, w);
    maxDepth = getNumber(s, w);
    skipWhiteSpace(s, w);
    matchString(s, w, "]");
    skipWhiteSpace(s, w);
  } else {
    minDepth = maxDepth = 1;
  }
  skipWhiteSpace(s, w);
  matchString(s, w, "in");
  skipWhiteSpace(s, w);
  rewriteSystemName = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ":");
  skipWhiteSpace(s, w);
  ConstrainedTerm ct = parseConstrainedTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");

  if (!contains(rewriteSystems, rewriteSystemName)) {
    cout << "Cannot find (constrained) rewrite system " << rewriteSystemName << endl;
    return;
  }
  RewriteSystem rs = rewriteSystems[rewriteSystemName];

  LOG(DEBUG3, cerr << "Narrowing search from " << toString(ct) << ".");
  vector<SmtSearchSolution> solutions = prune(smtSearchRewriteSystem(ct, rs, minDepth, maxDepth));
  cout << "Success: " << solutions.size() << " solutions." << endl;
  for (int i = 0; i < (int)solutions.size(); ++i) {
    cout << "Solution #" << i + 1 << ":" << endl;
    // TODO: simplify constrained term
    cout << toString(solutions[i]) << endl;
  }
}

void processUnify(string &s, int &w)
{
  matchString(s, w, "unify");
  skipWhiteSpace(s, w);
  FastTerm t1 = parseTerm(s, w);
  skipWhiteSpace(s, w);
  FastTerm t2 = parseTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");

  cout << "Unifying " << toString(t1) << " and " << toString(t2) << endl;

  vector<FastSubst> unifiers = unify(t1, t2);
  if (unifiers.size() == 0) {
    cout << "No solution." << endl;
  } else {
    cout << "Solution count: " << unifiers.size() << endl;
    for (uint i = 0; i < unifiers.size(); ++i) {
      cout << "Solution #" << i + 1 << ": ";
      cout << toString(unifiers[i]) << endl;
    }
  }
}

void processSmtUnify(string &s, int &w)
{
  matchString(s, w, "smt-unify");
  skipWhiteSpace(s, w);
  FastTerm t1 = parseTerm(s, w);
  skipWhiteSpace(s, w);
  FastTerm t2 = parseTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");

  cout << "Unifying " << toString(t1) << " and " << toString(t2) << endl;

  vector<SmtUnifySolution> unifiers = smtUnify(t1, t2);
  if (unifiers.size() == 0) {
    cout << "No solution." << endl;
  } else {
    cout << "Solution count: " << unifiers.size() << endl;
    for (uint i = 0; i < unifiers.size(); ++i) {
      cout << "Solution #" << i + 1 << ": ";
      cout << toString(unifiers[i]) << endl;
    }
  }
}

void processSatisfiability(string &s, int &w)
{
  matchString(s, w, "satisfiability");
  skipWhiteSpace(s, w);
  FastTerm constraint = parseTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");

  Z3_context context = init_z3_context();
  Z3_lbool result = z3_sat_check(context, constraint);
  
  if (result == Z3_L_TRUE) {
    cout << "The constraint " << toString(constraint) << " is SAT." << endl;
  } else if (result == Z3_L_FALSE) {
    cout << "The constraint " << toString(constraint) << " is UNSAT." << endl;
  } else {
    assert(result == Z3_L_UNDEF);
    cout << "Satisfiability check of constraint " << toString(constraint) << " is not conclusive." << endl;
  }
}
