#include "queryprovesim.h"
#include "parse.h"
#include "log.h"
#include "constrainedrewritesystem.h"
#include "rewritesystem.h"
#include "factories.h"
#include "term.h"
#include "funterm.h"
#include <string>
#include <cassert>
#include <queue>
using namespace std;

QueryProveSim::QueryProveSim() {
}

Query *QueryProveSim::create() {
  return new QueryProveSim();
}

ConstrainedRewriteSystem parseCRSfromName(string &s, int &w) {
  skipWhiteSpace(s, w);
  string crsName = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  if (!existsConstrainedRewriteSystem(crsName)) {
    Log(ERROR) << "No CRS exists with name " << crsName << endl;
    expected("Existing CRS", w, s);
  }
  return getConstrainedRewriteSystem(crsName);
}

void QueryProveSim::parse(std::string &s, int &w) {
  matchString(s, w, "show-simulation");
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
    matchString(s, w, "]");
    skipWhiteSpace(s, w);
  }
  else {
    maxDepth = 100;
  }
  matchString(s, w, "in");
  crsLeft = parseCRSfromName(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, "and");
  crsRight = parseCRSfromName(s, w);
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
    }
    else {
      break;
    }
  } while (1);

  matchString(s, w, "with-base");
  do {
    ConstrainedTerm ct = parseConstrainedTerm(s, w);
    base.push_back(ct);
    if (lookAhead(s, w, ",")) {
      matchString(s, w, ",");
      skipWhiteSpace(s, w);
    }
    else {
      break;
    }
  } while (1);
  matchString(s, w, ";");
}

// returns the constraint c such that we have arrived with current into base
Term *QueryProveSim::whenImpliesBase(ConstrainedTerm current) {
  Term *constraintResult = bFalse();
  for (int i = 0; i < (int)base.size(); ++i) {
    Term *constraint = current.normalizeFunctions().whenImplies(base[i]);
    constraintResult = bOr(constraintResult, constraint);
  }
  return constraintResult;
}

// returns the constraint c such that we have arrived with current into a circularity
Term *QueryProveSim::whenImpliesCircularity(ConstrainedTerm current) {
  Term *constraintResult = bFalse();
  for (int i = 0; i < (int)circularities.size(); ++i) {
    Term *constraint = current.normalizeFunctions().whenImplies(circularities[i]);
    constraintResult = bOr(constraintResult, constraint);
  }
  return constraintResult;
}

bool QueryProveSim::possibleCircularity(ConstrainedTerm ct) {
  for (int i = 0; i < (int)circularities.size(); ++i) {
    if (ct.normalizeFunctions().whenImplies(circularities[i]) != bFalse()) {
      return true;
    }
  }
  return false;
}

void QueryProveSim::decomposeConstrainedTermEq(ConstrainedTerm ct, Term *&lhs, Term *&rhs) {
  if (!ct.term->isFunTerm()) {
    Log(ERROR) << "Expected a pair as top-most function symbol (found variable instead)." << endl;
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
  assert(term->arguments.size() == 2);
  lhs = term->arguments[0];
  rhs = term->arguments[1];
}

bool QueryProveSim::possibleLhsBase(Term *lhs) {
  for (int i = 0; i < (int)base.size(); ++i) {
    Term *lhsBase, *rhsBase;
    QueryProveSim::decomposeConstrainedTermEq(base[i], lhsBase, rhsBase);
    Log(DEBUG) << "possibleLhsBase? Checking whether " << lhs->toString() << " unifies with " << lhsBase->toString() << endl;
    if (ConstrainedTerm(lhs, bTrue()).normalizeFunctions().whenImplies(ConstrainedTerm(lhsBase, bTrue())) != bFalse()) {
      Log(DEBUG) << "possibleLhsBase?     Is true that " << lhs->toString() << " unifies with " << lhsBase->toString() << endl;
      return true;
    }
    Log(DEBUG) << "possibleLhsBase?    Not true that " << lhs->toString() << " unifies with " << lhsBase->toString() << endl;
  }
  return false;
}

bool QueryProveSim::possibleRhsBase(Term *rhs) {
  for (int i = 0; i < (int)base.size(); ++i) {
    Term *lhsBase, *rhsBase;
    QueryProveSim::decomposeConstrainedTermEq(base[i], lhsBase, rhsBase);
    if (ConstrainedTerm(rhs, bTrue()).normalizeFunctions().whenImplies(ConstrainedTerm(rhsBase, bTrue())) != bFalse()) {
      return true;
    }
  }
  return false;
}

bool QueryProveSim::possibleLhsCircularity(Term *lhs) {
  for (int i = 0; i < (int)circularities.size(); ++i) {
    Term *lhsCircularity, *rhsCircularity;
    QueryProveSim::decomposeConstrainedTermEq(circularities[i], lhsCircularity, rhsCircularity);
    if (ConstrainedTerm(lhs, bTrue()).normalizeFunctions().whenImplies(ConstrainedTerm(lhsCircularity, bTrue())) != bFalse()) {
      return true;
    }
  }
  return false;
}

bool QueryProveSim::possibleRhsCircularity(Term *) {
  // TODO: check why this code is commented out
  // //  Term *lhs, *rhs;
  // //  QueryProveSim::decomposeConstrainedTermEq(ct, lhs, rhs);
  // for (int i = 0; i < (int)circularities.size(); ++i) {
  //   Term *lhsCircularity, *rhsCircularity;
  //   QueryProveSim::decomposeConstrainedTermEq(circularities[i], lhsCircularity, rhsCircularity);
  //   Substitution subst;
  //   Term *constraint;
  //   if (rhsCircularity->unifyModuloTheories(rhs, subst, constraint)) {
  //     return true;
  //   }
  // }
  return false;
}

ConstrainedTerm QueryProveSim::pairC(Term *left, Term *right, Term *constraint) {
  return simplifyConstrainedTerm(ConstrainedTerm(getFunTerm(pairFun, vector2(left, right)), constraint));
}

//returns a constraint under which either base or circularity holds
Term *QueryProveSim::proveBaseCase(ConstrainedTerm ct, bool progressLeft, bool progressRight, int depth) {
  ct = ct.normalizeFunctions();
  cout << spaces(depth) << "Trying to prove base case: " << ct.toString() << endl;
  Term *baseConstraint = simplifyConstraint(whenImpliesBase(ct));
  if(isSatisfiable(bNot(baseConstraint)) == unsat) {
    cout << spaces(depth) << "Proof succeeded: reached based equivalence." << endl;
    return bTrue();
  }
  cout << spaces(depth) << "Instance of base only when " + baseConstraint->toString() << endl;

  Term *circConstraint = bFalse();
  if (progressLeft && progressRight) {
    Log(DEBUG5) << spaces(depth) << "Testing for circularity" << endl;
    circConstraint = simplifyConstraint(whenImpliesCircularity(ct));
    if (isSatisfiable(bNot(circConstraint)) == unsat) {
      cout << spaces(depth) << "Proof succeeded: reached a circularity." << endl;
      return bTrue();
    }
    cout << spaces(depth) << "Instance of circularity only when " + circConstraint->toString() << endl;
    Log(DEBUG5) << spaces(depth) << "Instance of circularity only when (SMT) " + circConstraint->toString() << endl;
  }

  Term *constraint = simplifyConstraint(bOr(baseConstraint, circConstraint));
  if (isSatisfiable(bNot(constraint)) == unsat) {
    cout << spaces(depth) << "Proof succeeded: either base or circularity is reached in every case." << endl;
    return bTrue();
  }
  return constraint;
}

proveSimulationExistsRight_arguments::proveSimulationExistsRight_arguments(ConstrainedTerm ct, bool progressRight, int depth) :
  ct(ct), progressRight(progressRight), depth(depth) {
}

//returns a constraint under which RHS is unsolved
Term *QueryProveSim::proveSimulationExistsRight(proveSimulationExistsRight_arguments args, bool progressLeft) {
  
  Term *unsolvedConstraint = bTrue();

  queue<proveSimulationExistsRight_arguments> BFS_Q;
  BFS_Q.push(args);
  int initial_depth = args.depth;

  while (!BFS_Q.empty()) {

    proveSimulationExistsRight_arguments t = BFS_Q.front();
    BFS_Q.pop();

    if (t.depth > maxDepth) {
      cout << spaces(t.depth) << "! proof failed (exceeded maximum depth) exists right " << t.ct.toString() << endl;
      continue;
    }

    //adding unsolvedConstraint to constraint of current successor
    t.ct = ConstrainedTerm(t.ct.term, simplifyConstraint(bAnd(t.ct.constraint, unsolvedConstraint)));

    Term *lhs, *rhs;
    QueryProveSim::decomposeConstrainedTermEq(t.ct, lhs, rhs);

    cout << spaces(t.depth) << "+ prove exists right " << t.ct.toString() << endl;
    if ((possibleLhsBase(lhs) && possibleRhsBase(rhs)) || (progressLeft && t.progressRight && possibleCircularity(t.ct))) {
      Log(DEBUG5) << spaces(t.depth) << "possible rhs base" << endl;
      Term *baseCaseConstraint = proveBaseCase(t.ct, progressLeft, t.progressRight, t.depth + 1);
      if (baseCaseConstraint == bTrue()) {
        cout << spaces(t.depth) << "- proof successful exists right " << t.ct.toString() << endl;
        for (--t.depth; t.depth >= initial_depth; --t.depth)
          cout << spaces(t.depth) << "- proof was successful for exists right" << endl;
        return bFalse();
      }
      Log(DEBUG5) << spaces(t.depth) << "close, but no cigar" << endl;
      //if baseCaseConstraint holds, then the problem is solved
      unsolvedConstraint = simplifyConstraint(bAnd(unsolvedConstraint, bNot(baseCaseConstraint)));

      if (isSatisfiable(unsolvedConstraint) == unsat) {
        cout << spaces(t.depth) << "- proof successful exists right (no unsolved cases) " << unsolvedConstraint->toString() << endl;
        return bFalse();
      }

      //re-adding unsolvedConstrained to current successor
      t.ct = ConstrainedTerm(t.ct.term, simplifyConstraint(bAnd(t.ct.constraint, unsolvedConstraint)));
      Log(DEBUG5) << spaces(t.depth) << "+ continuing exists right for case " << unsolvedConstraint->toString() << endl;
    }

    vector< pair<ConstrainedSolution, bool> > rhsSuccessors;
    for (const auto &it : ConstrainedTerm(rhs, t.ct.constraint).smtNarrowSearch(crsRight))
      rhsSuccessors.push_back(make_pair(it, true));
    if (rhsSuccessors.size() == 0) {
      cout << spaces(t.depth) << "no rhs successors, taking defined symbols" << "(" << ConstrainedTerm(rhs, t.ct.constraint).toString() << ")" << endl;
      for (const auto &it : ConstrainedTerm(rhs, t.ct.constraint).smtNarrowDefinedSearch())
        rhsSuccessors.push_back(make_pair(it, false));
    }
    if (rhsSuccessors.size() == 0) {
      cout << spaces(t.depth) << "! proof failed (no successors) exists right " << t.ct.toString() << endl;
      continue;
    }
    for (int i = 0; i < (int)rhsSuccessors.size(); ++i) {
      ConstrainedSolution sol = rhsSuccessors[i].first;
      ConstrainedTerm afterStep = pairC(lhs, sol.term, bAnd(t.ct.constraint, sol.constraint));
      afterStep = afterStep.substitute(sol.subst).substitute(sol.simplifyingSubst);
      afterStep = simplifyConstrainedTerm(afterStep);
      BFS_Q.push(proveSimulationExistsRight_arguments(afterStep, rhsSuccessors[i].second, t.depth + 1));
    }
    
  }

  return unsolvedConstraint;
}

bool QueryProveSim::proveSimulationForallLeft(ConstrainedTerm ct, bool progressLeft, bool progressRight, int depth) {
  if (depth > maxDepth) {
    if (!progressLeft) {
      cout << spaces(depth) << "! reached a forall left state for which no progress was made " << ct.toString() << endl;
      return true;
    }
    cout << spaces(depth) << "! proof failed (exceeded maximum depth) forall left " << ct.toString() << endl;
    return false;
  }
  Term *lhs, *rhs;
  QueryProveSim::decomposeConstrainedTermEq(ct, lhs, rhs);

  cout << spaces(depth) << "+ prove forall left " << ct.toString() << endl;
  Log(DEBUG5) << spaces(depth) << "possible lhs base " << possibleLhsBase(lhs) << endl;
  Log(DEBUG5) << spaces(depth) << "progressLeft " << progressLeft << "; possibleLhsCircularity " << possibleLhsCircularity(lhs) << endl;
  Term *unsolvedConstraint = bTrue();
  if (possibleLhsBase(lhs) || (progressLeft && possibleLhsCircularity(lhs))) {
    Log(DEBUG5) << spaces(depth) << "possible lhs base or circularity" << endl;
    // TODO: changed progressRight to true for partial equivalence temporarily
    unsolvedConstraint = proveSimulationExistsRight(proveSimulationExistsRight_arguments(ct, true /* progressRight */, depth + 1), progressLeft);
    if (unsolvedConstraint == bFalse()) {
      cout << spaces(depth) << "- proof succeeded forall left " << ct.toString() << endl;
      return true;
    }
    Log(DEBUG5) << spaces(depth) << "close, but no cigar" << endl;
    Log(DEBUG5) << spaces(depth) << "continuing forall left for case " << unsolvedConstraint->toString() << endl;
  }

  ct = ConstrainedTerm(ct.term, simplifyConstraint(bAnd(ct.constraint, unsolvedConstraint)));

  vector< pair<ConstrainedSolution, bool> > lhsSuccessors;
  for (const auto &it : ConstrainedTerm(lhs, ct.constraint).smtNarrowSearch(crsLeft))
    lhsSuccessors.push_back(make_pair(it, true));
  if (lhsSuccessors.size() == 0) {
    cout << spaces(depth) << "no lhs successors, taking defined symbols" << "(" << ConstrainedTerm(lhs, ct.constraint).toString() << ")" << endl;
    for (const auto &it : ConstrainedTerm(lhs, ct.constraint).smtNarrowDefinedSearch())
      lhsSuccessors.push_back(make_pair(it, false));
  }
  for (int i = 0; i < (int)lhsSuccessors.size(); ++i) {
    ConstrainedSolution sol = lhsSuccessors[i].first;
    ConstrainedTerm afterStep = pairC(sol.term, rhs, bAnd(ct.constraint, sol.constraint));
    afterStep = simplifyConstrainedTerm(afterStep.substitute(sol.subst).substitute(sol.simplifyingSubst));
    if (!proveSimulationForallLeft(afterStep, lhsSuccessors[i].second, progressRight, depth + 1)) {
      cout << spaces(depth) << "! proof failed (" << i << "th successor) forall left " << ct.toString() << endl;
      return false;
    }
  }
  if (lhsSuccessors.size() > 0) {
    cout << spaces(depth) << "- proof succeeded forall left " << ct.toString() << endl;
    return true;
  }
  else {
    cout << spaces(depth) << "- proof failed forall left (no successors) " << ct.toString() << endl;
    return false;
  }
}

bool QueryProveSim::proveSimulation(ConstrainedTerm ct, bool progressLeft, bool progressRight, int depth) {
  ct = ct.normalizeFunctions();
  cout << spaces(depth) << "Proving simulation circularity " << ct.toString() << endl;
  bool result = proveSimulationForallLeft(ct, progressLeft, progressRight, depth + 1);
  if (result) {
    cout << spaces(depth) << "Proof succeeded." << endl;
  }
  else {
    cout << spaces(depth) << "Proof failed." << endl;
  }
  return result;
}

void QueryProveSim::execute() {
  Log(DEBUG9) << "Proving equivalence" << endl;
  Log(DEBUG9) << "Left constrained rewrite sytem:" << endl;
  Log(DEBUG9) << crsLeft.toString() << endl;
  Log(DEBUG9) << "Rigth constrained rewrite sytem:" << endl;
  Log(DEBUG9) << crsRight.toString() << endl;
  Log(DEBUG9) << "Base" << endl;
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
    }
    else {
      Log(ERROR) << "Base terms must all start with the same pairing symbol." << endl;
      Log(ERROR) << "Expecting terms to start with " << pairFun->name << ", but found " << funTerm->toString() << "." << endl;
      abort();
    }
  }
  if (pairFun == 0) {
    Log(ERROR) << "Found no base terms in equivalence prove query." << endl;
    abort();
  }

  int circCount = static_cast<int>(circularities.size());
  // expand all defined functions in circularities (twice)
  for (int i = 0; i < circCount; ++i) {
    ConstrainedTerm ct = circularities[i];
    vector<ConstrainedTerm> csols = ct.smtNarrowDefinedSearch(1, 1);
    for (int j = 0; j < static_cast<int>(csols.size()); ++j) {
      ConstrainedTerm newBase = csols[j];
      vector<ConstrainedTerm> csols2 = newBase.smtNarrowDefinedSearch(1, 1);
      for (int k = 0; k < static_cast<int>(csols2.size()); ++k) {
        ConstrainedTerm newnewBase = csols2[k];
        Log(INFO) << "Adding new circ (not necessary to prove) " << newnewBase.toString() << endl;
        circularities.push_back(newnewBase);
      }
      Log(INFO) << "Adding new circ (not necessary to prove) " << newBase.toString() << endl;
      circularities.push_back(newBase);
    }
  }

  // prove all circularities
  for (int i = 0; i < circCount; ++i) {
    cout << "Proving equivalence circularity #" << (i + 1) << endl;
    ConstrainedTerm ct = circularities[i];
    if (proveSimulation(ct, false, false, 0)) {
      cout << "Succeeded in proving circularity #" << (i + 1) << endl;
    }
    else {
      cout << "Failed to prove circularity #" << (i + 1) << endl;
    }
  }
}
