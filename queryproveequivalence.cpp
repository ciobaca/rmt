#include "queryproveequivalence.h"
#include "constrainedterm.h"
#include "parse.h"
#include "factories.h"
#include "funterm.h"
#include "z3driver.h"
#include "log.h"
#include "sort.h"
#include <string>
#include <sstream>
#include <map>
#include <iostream>
#include <cassert>

using namespace std;

QueryProveEquivalence::QueryProveEquivalence()
{
}
  
Query *QueryProveEquivalence::create()
{
  return new QueryProveEquivalence();
}

void QueryProveEquivalence::parse(std::string &s, int &w)
{
  matchString(s, w, "show-equivalent");
  skipWhiteSpace(s, w);
  if (lookAhead(s, w, "[")) {
    matchString(s, w, "[");
    skipWhiteSpace(s, w);
    maxDepth = getNumber(s, w);
    if (maxDepth < 0 || maxDepth > 99999) {
      Log(ERROR) << "Maximum depth (" << maxDepth << ") must be between 0 and 99999" << endl;
      expected("Legal maximum depth", w, s);
    }
    skipWhiteSpace(s, w);
    matchString(s, w, ",");
    skipWhiteSpace(s, w);
    maxBranchingDepth = getNumber(s, w);
    if (maxBranchingDepth < 0 || maxBranchingDepth > 99999) {
      Log(ERROR) << "Maximum branching depth (" << maxBranchingDepth << ") must be between 0 and 99999" << endl;
      expected("Legal maximum branching depth", w, s);
    }
    skipWhiteSpace(s, w);
    matchString(s, w, "]");
    skipWhiteSpace(s, w);
  } else {
    maxDepth = 100;
    maxBranchingDepth = 2;
  }
  matchString(s, w, "in");
  skipWhiteSpace(s, w);
  lrsName = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, "and");
  skipWhiteSpace(s, w);
  rrsName = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ":");
  skipWhiteSpace(s, w);

  do {
    ConstrainedTerm ct = parseConstrainedTerm(s, w);
    circularities.push_back(ct);
    skipWhiteSpace(s, w);
    if (lookAhead(s, w, ",")) {
      matchString(s, w, ",");
      skipWhiteSpace(s, w);
    } else {
      break;
    }
  } while(1);

  matchString(s, w, "with-base");
  do {
    ConstrainedTerm ct = parseConstrainedTerm(s, w);
    base.push_back(ct);
    if (lookAhead(s, w, ",")) {
      matchString(s, w, ",");
      skipWhiteSpace(s, w);
    } else {
      break;
    }
  } while(1);
  matchString(s, w, ";");
}

// assume current = <t1 if c1>
// assume goal = <t2 if c2>
// returns a constraint c such that
//
// forall X, c(X) /\ c1(X) -> exists Y, s.t. c2(X, Y) /\ t1(X) = t2(X,Y)
// 
// X are all variables in <t1 if c1>
// Y are all variables in <t2 if c2>
// Term *whenImplies(ConstrainedTerm current, ConstrainedTerm goal)
// {
// }

// // returns the constraint c such that we have arrived with current into base
// Term *QueryProveEquivalence::whenImpliesBase(ConstrainedTerm current)
// {
//   Term *constraintResult = bTrue();
//   for (int i = 0; i < (int)base.size(); ++i) {
//     Term *constraint = whenImplies(current, base[i]);
//     constraintResult = bOr(constraintResult, constraint);
//   }
//   return constraintResutl;
// }
  
void QueryProveEquivalence::execute()
{
  // cout << "TODO: Trying to prove equivalence." << endl;
  // CRewriteSystem crsLeft = getCRewriteSystem(lrsName);
  // CRewriteSystem crsRight = getCRewriteSystem(rrsName);
  // Log(DEBUG6) << "Proving equivalence" << endl;
  // Log(DEBUG6) << "Left constrained rewrite sytem:" << endl;
  // Log(DEBUG6) << crsLeft.toString() << endl;
  // Log(DEBUG6) << "Rigth constrained rewrite sytem:" << endl;
  // Log(DEBUG6) << crsRight.toString() << endl;
  // // prove all circularities
  // for (int i = 0; i < (int)circularities.size(); ++i) {
  //   ConstrainedTerm ct = circularities[i];

  //   Term *constraint = whenImpliesBase(ct);
  // }
}
