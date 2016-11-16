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

QueryProveEquivalence::QueryProveEquivalence() : pairFun(0)
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

// returns the constraint c such that we have arrived with current into a circularity
Term *QueryProveEquivalence::whenImpliesCircularity(ConstrainedTerm current)
{
  Term *constraintResult = bFalse();
  for (int i = 0; i < (int)circularities.size(); ++i) {
    Term *constraint = current.whenImplies(circularities[i]);
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
    Term *constraint;
    if (lhsBase->unifyModuloTheories(lhs, subst, constraint)) {
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
    decomposeConstrainedTermEq(base[i], lhsBase, rhsBase);
    Substitution subst;
    Term *constraint;
    if (rhsBase->unifyModuloTheories(rhs, subst, constraint)) {
      return true;
    }
  }
  return false;
}

bool QueryProveEquivalence::possibleLhsCircularity(Term *lhs)
{
  //  Term *lhs, *rhs;
  //  decomposeConstrainedTermEq(ct, lhs, rhs);
  for (int i = 0; i < (int)circularities.size(); ++i) {
    Term *lhsCircularity, *rhsCircularity;
    decomposeConstrainedTermEq(circularities[i], lhsCircularity, rhsCircularity);
    Substitution subst;
    Term *constraint;
    if (lhsCircularity->unifyModuloTheories(lhs, subst, constraint)) {
      return true;
    }
  }
  return false;
}

bool QueryProveEquivalence::possibleRhsCircularity(Term *rhs)
{
  //  Term *lhs, *rhs;
  //  decomposeConstrainedTermEq(ct, lhs, rhs);
  for (int i = 0; i < (int)circularities.size(); ++i) {
    Term *lhsCircularity, *rhsCircularity;
    decomposeConstrainedTermEq(circularities[i], lhsCircularity, rhsCircularity);
    Substitution subst;
    Term *constraint;
    if (rhsCircularity->unifyModuloTheories(rhs, subst, constraint)) {
      return true;
    }
  }
  return false;
}

bool QueryProveEquivalence::proveEquivalenceExistsRight(ConstrainedTerm ct, bool progressLeft, bool progressRight, int depth, int branchingDepth)
{
  Term *lhs, *rhs;
  decomposeConstrainedTermEq(ct, lhs, rhs);

  cout << spaces(depth) << "prove exists right " << ct.toString() << endl;
  if ((possibleLhsBase(lhs) && possibleRhsBase(rhs)) || (progressLeft && progressRight && possibleRhsCircularity(rhs) && possibleLhsCircularity(lhs))) {
    Log(DEBUG5) << spaces(depth) << "possible rhs base" << endl;
    if (proveEquivalence(ct, progressLeft, progressRight, depth + 1, branchingDepth + 1)) {
      cout << spaces(depth) << "proof successful exists right " << ct.toString() << endl;
      return true;
    }
    Log(DEBUG5) << spaces(depth) << "close, but no cigar" << endl;
  }
  if (depth > maxDepth) {
    cout << spaces(depth) << "proof failed (exceeded maximu depth) exists right " << ct.toString() << endl;
    return false;
  }
  vector<ConstrainedTerm> rhsSuccessors = ConstrainedTerm(rhs, ct.constraint).smtNarrowSearch(crsRight, 1, 1);
  for (int i = 0; i < (int)rhsSuccessors.size(); ++i) {
    if (proveEquivalenceExistsRight(pairC(lhs, rhsSuccessors[i].term, bAnd(ct.constraint, rhsSuccessors[i].constraint)), progressLeft, true, depth + 1, branchingDepth)) {
      cout << spaces(depth) << "proof succeeded (" << i << "th successor) exists right" << ct.toString() << endl;
      return true;
    }
  }
  cout << spaces(depth) << "proof failed (no case went through) exists right " << ct.toString() << endl;
  return false;
}

bool QueryProveEquivalence::proveEquivalenceForallLeft(ConstrainedTerm ct, bool progressLeft, bool progressRight, int depth, int branchingDepth)
{
  Term *lhs, *rhs;
  decomposeConstrainedTermEq(ct, lhs, rhs);

  cout << spaces(depth) << "prove forall left " << ct.toString() << endl;
  Log(DEBUG5) << spaces(depth) << "possible lhs base " << possibleLhsBase(lhs) << endl;
  Log(DEBUG5) << spaces(depth) << "progressLeft " << progressLeft << "; possibleLhsCircularity " << possibleLhsCircularity(lhs) << endl;
  if (possibleLhsBase(lhs) || (progressLeft && possibleLhsCircularity(lhs))) {
    Log(DEBUG5) << spaces(depth) << "possible lhs base or circularity" << endl;
    if (proveEquivalenceExistsRight(ct, progressLeft, progressRight, depth + 1, branchingDepth)) {
      cout << spaces(depth) << "proof succeeded forall left " << ct.toString() << endl;
      return true;
    }
    Log(DEBUG5) << spaces(depth) << "close, but no cigar" << endl;
  }
  if (depth > maxDepth) {
    Log(DEBUG5) << spaces(depth) << "proof failed (exceeded maximum depth) forall left " << ct.toString() << endl;
    return false;
  }
  vector<ConstrainedTerm> lhsSuccessors = ConstrainedTerm(lhs, ct.constraint).smtNarrowSearch(crsLeft, 1, 1);
  for (int i = 0; i < (int)lhsSuccessors.size(); ++i) {
    if (!proveEquivalenceForallLeft(pairC(lhsSuccessors[i].term, rhs, bAnd(ct.constraint, lhsSuccessors[i].constraint)), true, progressRight, depth + 1, branchingDepth)) {
      cout << spaces(depth) << "proof failed (" << i << "th successor) forall left " << ct.toString() << endl;
      return false;
    }
  }
  cout << spaces(depth) << "proof succeeded forall left " << ct.toString();
  return true;
}

bool QueryProveEquivalence::proveEquivalence(ConstrainedTerm ct, bool progressLeft, bool progressRight, int depth, int branchingDepth)
{
  cout << spaces(depth) << "Proving equivalence circularity " << ct.toString() << endl;
  Log(DEBUG5) << spaces(depth) << "Testing for base" << endl;
  Term *constraint = simplifyConstraint(whenImpliesBase(ct));
  if (constraint == bTrue()) {
    cout << spaces(depth) << "Proof succeeded: reached based equivalence." << endl; 
    return true;
  } else {
    Log(DEBUG5) << spaces(depth) << "Instance of base only when " + constraint->toString() << endl;
    if (progressLeft && progressRight) {
      Log(DEBUG5) << spaces(depth) << "Testing for circularity" << endl;
      constraint = simplifyConstraint(whenImpliesCircularity(ct));
      if (constraint == bTrue()) {
	cout << spaces(depth) << "Proof succeeded: reached based equivalence." << endl; 
	return true;
      }
      Log(DEBUG5) << spaces(depth) << "Instance of circularity only when " + constraint->toString() << endl;
    }
    if (branchingDepth > maxBranchingDepth) {
      cout << spaces(depth) << "Proof failed: branching depth limit exceeded." << endl;
      return false;
    }
    if (depth > maxDepth) {
      cout << spaces(depth) << "Proof failed: depth limit exceeded." << endl;
      return false;
    }
    bool result = proveEquivalenceForallLeft(ct, progressLeft, progressRight, depth + 1, branchingDepth);
    if (result) {
      cout << spaces(depth) << "Proof succeeded." << endl;
    } else {
      cout << spaces(depth) << "Proof failed." << endl;
    }
    return result;
  }
}

Term *QueryProveEquivalence::pair(Term *left, Term *right)
{
  return getFunTerm(pairFun, vector2(left, right));
}

ConstrainedTerm QueryProveEquivalence::pairC(Term *left, Term *right, Term *constraint)
{
  return simplifyConstrainedTerm(ConstrainedTerm(pair(left, right), constraint));
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
  Log(DEBUG6) << "Base" << endl;
  pairFun = 0;
  for (int i = 0; i < (int)base.size(); ++i) {
    Log(DEBUG6) << base[i].toString() << endl;
    if (!base[i].term->isFunTerm()) {
      Log(ERROR) << "Base terms must start with a pairing symbol (is variable now)." << endl;
      Log(ERROR) << base[i].toString() << endl;
      abort();
    }
    FunTerm *funTerm = base[i].term->getAsFunTerm();
    if (funTerm->arguments.size() != 2) {
      Log(ERROR) << "Base terms must start with a pairing symbol (but wrong arity)." << endl;
      Log(ERROR) << base[i].toString() << endl;
      abort();
    }
    if (pairFun == 0 || pairFun == funTerm->function) {
      pairFun = funTerm->function;
      continue;
    } else {
      Log(ERROR) << "Base terms must all start with the same pairing symbol." << endl;
      Log(ERROR) << "Expecting terms to start with " << pairFun->name << ", but found " << funTerm->toString() << "." << endl;
      abort();
    }
  }
  if (pairFun == 0) {
    Log(ERROR) << "Found no base terms in equivalence prove query." << endl;
    abort();
  }

  // prove all circularities
  for (int i = 0; i < (int)circularities.size(); ++i) {
    cout << "Proving equivalence circularity #" << (i + 1) << endl;
    ConstrainedTerm ct = circularities[i];
    if (proveEquivalence(ct, false, false, false, false)) {
      cout << "Succeeded in proving circularity #" << (i + 1) << endl;
    } else {
      cout << "Failed to prove circularity #" << (i + 1) << endl;
    }
  }
}
