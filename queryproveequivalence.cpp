#include "queryproveequivalence.h"
#include "constrainedterm.h"
#include "parse.h"
#include "factories.h"
#include "funterm.h"
#include "z3driver.h"
#include "log.h"
#include "sort.h"
#include "helper.h"
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

// returns the constraint c such that we have arrived with current into base
Term *QueryProveEquivalence::whenImpliesBase(ConstrainedTerm current)
{
  Term *constraintResult = bFalse();
  for (int i = 0; i < (int)base.size(); ++i) {
    Term *constraint = current.whenImplies(base[i]);
    constraintResult = bOr(constraintResult, constraint);
  }
  return constraintResult;
}

void decomposeConstrainedTermEq(ConstrainedTerm ct, Term *&lhs, Term *&rhs)
{
  if (!ct.term->isFunTerm()) {
    Log(ERROR) << "Expecting a pair as top-most function symbol (found variable instead)." << endl;
    Log(ERROR) << ct.toString() << endl;
    abort();
  }
  assert(ct.term->isFunTerm());
  FunTerm *term = ct.term->getAsFunTerm();
  if (term->arguments.size() != 2) {
    Log(ERROR) << "Expecting a pair as top-most function symbol in base equivalence (found function symbol of wrong arity instead)." << endl;
    Log(ERROR) << term->toString() << endl;
    abort();
  }
  assert(arguments.size() == 2);
  lhs = term->arguments[0];
  rhs = term->arguments[1];
}

bool QueryProveEquivalence::possibleLhsBase(Term *lhs)
{
  //  Term *lhs, *rhs;
  //  decomposeConstrainedTermEq(ct, lhs, rhs);
  for (int i = 0; i < (int)base.size(); ++i) {
    Term *lhsBase, *rhsBase;
    decomposeConstrainedTermEq(base[i], lhsBase, rhsBase);
    Substitution subst;
    if (lhsBase->unifyWith(lhs, subst)) {
      return true;
    }
  }
  return false;
}

bool QueryProveEquivalence::possibleRhsBase(Term *rhs)
{
  //  Term *lhs, *rhs;
  //  decomposeConstrainedTermEq(ct, lhs, rhs);
  for (int i = 0; i < (int)base.size(); ++i) {
    Term *lhsBase, *rhsBase;
    Substitution subst;
    decomposeConstrainedTermEq(base[i], lhsBase, rhsBase);
    if (rhsBase->unifyWith(rhs, subst)) {
      return true;
    }
  }
  return false;
}

void QueryProveEquivalence::proveEquivalenceExistsRight(ConstrainedTerm ct, bool progress, int depth)
{
  Term *lhs, *rhs;
  decomposeConstrainedTermEq(ct, lhs, rhs);

  Log(DEBUG5) << spaces(depth) << "exists right " << ct.toString() << endl;
  if (possibleRhsBase(rhs)) {
    //    proveEquivalenceExistsRight(ct, progress, depth + 1);
  } else {
    vector<ConstrainedTerm> lhsSuccessors = ConstrainedTerm(lhs, ct.constraint).smtNarrowSearch(crsLeft, 1, 1);
    for (int i = 0; i < (int)lhsSuccessors.size(); ++i) {
      proveEquivalenceExistsRight(ConstrainedTerm(lhsSuccessors[i].term, bAnd(ct.constraint, lhsSuccessors[i].constraint)), progress, depth + 1);
    }
  }
}

void QueryProveEquivalence::proveEquivalenceForallLeft(ConstrainedTerm ct, bool progress, int depth)
{
  Term *lhs, *rhs;
  decomposeConstrainedTermEq(ct, lhs, rhs);

  Log(DEBUG5) << spaces(depth) << "forall left " << ct.toString() << endl;
  if (possibleLhsBase(lhs)) {
    proveEquivalenceExistsRight(ct, progress, depth + 1);
  } else {
    vector<ConstrainedTerm> lhsSuccessors = ConstrainedTerm(lhs, ct.constraint).smtNarrowSearch(crsLeft, 1, 1);
    for (int i = 0; i < (int)lhsSuccessors.size(); ++i) {
      proveEquivalenceForallLeft(ConstrainedTerm(lhsSuccessors[i].term, bAnd(ct.constraint, lhsSuccessors[i].constraint)), progress, depth + 1);
    }
  }
}

void QueryProveEquivalence::proveEquivalence(ConstrainedTerm ct, bool progress, int depth)
{
  cout << spaces(depth) << "Proving equivalence circularity" << ct.toString() << endl;
  Term *constraint = simplifyConstraint(whenImpliesBase(ct));
  if (constraint == bTrue()) {
    cout << spaces(depth) << "Reached base equivalence, done";
  }
  if (constraint != bTrue()) {
    cout << spaces(depth) << "Did not reach base equivalence, continuing";
    proveEquivalenceForallLeft(ct, progress, depth);
  }
}

void QueryProveEquivalence::execute()
{
  crsLeft = getCRewriteSystem(lrsName);
  crsRight = getCRewriteSystem(rrsName);

  Log(DEBUG6) << "Proving equivalence" << endl;
  Log(DEBUG6) << "Left constrained rewrite sytem:" << endl;
  Log(DEBUG6) << crsLeft.toString() << endl;
  Log(DEBUG6) << "Rigth constrained rewrite sytem:" << endl;
  Log(DEBUG6) << crsRight.toString() << endl;
  // prove all circularities
  for (int i = 0; i < (int)circularities.size(); ++i) {
    Log(DEBUG6) << "Proving equivalence circularity #" << (i + 1) << endl;
    ConstrainedTerm ct = circularities[i];
    proveEquivalence(ct, false, 0);
  }
}
