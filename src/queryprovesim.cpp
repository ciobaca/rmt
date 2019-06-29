#include "queryprovesim.h"
#include "constrainedterm.h"
#include "parse.h"
#include "fastterm.h"
#include "log.h"
#include "helper.h"
#include "rewritesystem.h"
#include "smtunify.h"
#include "search.h"
#include <string>
#include <sstream>
#include <map>
#include <iostream>
#include <cassert>

extern map<string, RewriteSystem> rewriteSystems;

QueryProveSim::QueryProveSim() {
  context = init_z3_context();
}

Query *QueryProveSim::create(){
  return new QueryProveSim();
}

void QueryProveSim::parse(std::string &s, int &w) {
  matchString(s, w, "show-simulation");
  skipWhiteSpace(s, w);

  /* defaults */
  proveTotal = false;
  useDFS = false;
  maxDepth = 100;
  isBounded = false;

  if (lookAhead(s, w, "useDFS")) {
    matchString(s, w, "useDFS");
    useDFS = true;
    skipWhiteSpace(s, w);
  }

  if (lookAhead(s, w, "[")) {
    matchString(s, w, "[");
    skipWhiteSpace(s, w);
    maxDepth = getNumber(s, w);
    if (maxDepth < 0 || maxDepth > 99999) {
      cout << "Maximum depth (" << maxDepth << ") must be between 0 and 99999" << endl;
      expected("Legal maximum depth", w, s);
    }
    skipWhiteSpace(s, w);
    if (lookAhead(s, w, ",")) {
      matchString(s, w, ",");
      skipWhiteSpace(s, w);
      if (lookAhead(s, w, "partial")) {
        matchString(s, w, "partial");
        proveTotal = false;
      }
      else if (lookAhead(s, w, "total")) {
        matchString(s, w, "total");
        proveTotal = true;
      }
      else {
        expected("Type of simulation must be either 'partial' or 'total'", w, s);
      }
    }

    skipWhiteSpace(s, w);
    if (lookAhead(s, w, ",")) {
      matchString(s, w, ",");
      skipWhiteSpace(s, w);
      if (lookAhead(s, w, "bounded")) {
        matchString(s, w, "bounded");
        isBounded = true;
      }
      else {
        expected("Word 'bounded'", w, s);
      }
    }
    skipWhiteSpace(s, w);
    matchString(s, w, "]");
    skipWhiteSpace(s, w);
  }
  matchString(s, w, "in");

  skipWhiteSpace(s, w);
  string crsLName = getIdentifier(s, w);
  if (!contains(rewriteSystems, crsLName)) {
    cout << "Cannot find rewrite system " << crsLName << endl;
    expected("Rewrite system name", w, s);
  }
  crsLeft = rewriteSystems[crsLName];
  skipWhiteSpace(s, w);

  matchString(s, w, "and");

  skipWhiteSpace(s, w);
  string crsRName = getIdentifier(s, w);
  if (!contains(rewriteSystems, crsRName)) {
    cout << "Cannot find rewrite system " << crsLName << endl;
    expected("Rewrite system name", w, s);
  }
  crsLeft = rewriteSystems[crsRName];
  skipWhiteSpace(s, w);
  
  matchString(s, w, ":");
  skipWhiteSpace(s, w);

  do {
    assumedCircularities.push_back(false);
    if (lookAhead(s, w, "[assumed]")) {
      matchString(s, w, "[assumed]");
      skipWhiteSpace(s, w);
      assumedCircularities.back() = true;
    }
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
  skipWhiteSpace(s, w);
  do {
    ConstrainedTerm ct = parseConstrainedTerm(s, w);
    base.push_back(ct);
    skipWhiteSpace(s, w);
    if (lookAhead(s, w, ",")) {
      matchString(s, w, ",");
      skipWhiteSpace(s, w);
    }
    else {
      break;
    }
  } while (1);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");
  skipWhiteSpace(s, w);


  LOG(DEBUG9, cout << "Base" << endl);
  pairFun = 0;
  for (int i = 0; i < (int)base.size(); ++i) {
    LOG(DEBUG6, cout << toString(base[i]) << endl);
    if (!isFuncTerm(base[i].term)) {
      LOG(ERROR, cerr << "Base terms must start with a pairing symbol (is variable now)." << endl);
      LOG(ERROR, cerr << toString(base[i]) << endl);
      abort();
    }
    FastTerm funTerm = base[i].term;
    if (getArity(getFunc(funTerm)) != 2) {
      LOG(ERROR, cerr << "Base terms must start with a pairing symbol (but wrong arity)." << endl);
      LOG(ERROR, cerr << toString(base[i]) << endl);
      abort();
    }
    if (pairFun == 0 || pairFun == getFunc(funTerm)) {
      pairFun = getFunc(funTerm);
    }
    else {
      LOG(ERROR, cerr << "Base terms must all start with the same pairing symbol." << endl);
      LOG(ERROR, cerr << "Expecting terms to start with " << getFuncName(pairFun) << ", but found " << toString(funTerm) << "." << endl);
      abort();
    }
  }
  if (pairFun == 0) {
    LOG(ERROR, cerr << "Found no base terms in simulation prove query." << endl);
    abort();
  }
}

extern map<string, RewriteSystem> rewriteSystems;
extern RewriteSystem rsDefinedCombined;

void QueryProveSim::decomposeConstrainedTermEq(ConstrainedTerm ct, FastTerm &lhs, FastTerm &rhs) {
  if (!isFuncTerm(ct.term)) {
    LOG(ERROR, cerr << "Expected a pair as top-most function symbol (found variable instead)." << endl);
    LOG(ERROR, cerr << toString(ct) << endl);
    abort();
  }
  FastTerm term = ct.term;
  FastFunc funct = getFunc(term);
  if (getArity(funct) != 2) {
    LOG(ERROR, cerr << "Expecting a pair as top-most function symbol in base equivalence (found function symbol of wrong arity instead)." << endl);
    LOG(ERROR, cerr << toString(term) << endl);
    abort();
  }
  lhs = getArg(term, 0);
  rhs = getArg(term, 1);
}

ConstrainedTerm QueryProveSim::pairC(FastTerm left, FastTerm right, FastTerm constraint) {
  FastTerm args[2];
  args[0] = left, args[1] = right;
  return simplify(ConstrainedTerm(newFuncTerm(pairFun, args), constraint));
}

bool QueryProveSim::possibleLhsBase(FastTerm lhs) {
  for (int i = 0; i < (int)base.size(); ++i) {
    FastTerm lhsBase, rhsBase;
    QueryProveSim::decomposeConstrainedTermEq(base[i], lhsBase, rhsBase);
    LOG(INFO, cout << "possibleLhsBase? Checking whether " << toString(lhs) << " unifies with " << toString(lhsBase) << endl);
    if (ConstrainedTerm(lhs, fastTrue()).whenImplies(ConstrainedTerm(lhsBase, fastTrue())) != fastFalse()) {
      LOG(INFO, cout << "possibleLhsBase?     Is true that " << toString(lhs) << " unifies with " << toString(lhsBase) << endl);
      return true;
    }
    LOG(INFO, cout << "possibleLhsBase?    Not true that " << toString(lhs) << " unifies with " << toString(lhsBase) << endl);
  }
  return false;
}

bool QueryProveSim::possibleRhsBase(FastTerm rhs) {
  for (int i = 0; i < (int)base.size(); ++i) {
    FastTerm lhsBase, rhsBase;
    QueryProveSim::decomposeConstrainedTermEq(base[i], lhsBase, rhsBase);
    if (ConstrainedTerm(rhs, fastTrue()).whenImplies(ConstrainedTerm(rhsBase, fastTrue())) != fastFalse()) {
      return true;
    }
  }
  return false;
}


bool QueryProveSim::proveSimulationForallLeft(ConstrainedTerm ct, bool progressLeft, int depth) {
  if (depth > maxDepth) {
    cout << spaces(depth) << "! proof failed (exceeded maximum depth) forall left " << toString(ct) << endl;
    return false;
  }
  FastTerm lhs = 0, rhs = 0;
  QueryProveSim::decomposeConstrainedTermEq(ct, lhs, rhs);

  cout << spaces(depth) << "+ prove forall left " << toString(ct) << endl;
  //TODO
  //LOG(DEBUG5, cout << spaces(depth) << "possible lhs base " << possibleLhsBase(lhs) << endl);
  //LOG(DEBUG5, cout << spaces(depth) << "progressLeft " << progressLeft << "; possibleLhsCircularity " << possibleLhsCircularity(lhs) << endl);
  FastTerm unsolvedConstraint = fastTrue();
  if (//possibleLhsBase(lhs) ||
    //TODO
    //(canApplyCircularities(progressLeft, true /*I COULD have progress right*/) && possibleLhsCircularity(lhs))
    true
    ) {
    LOG(DEBUG5, cout << spaces(depth) << "possible lhs base or circularity" << endl);
    //TODO
    //unsolvedConstraint = proveSimulationExistsRight(proveSimulationExistsRight_arguments(ct, false, depth + 1), progressLeft);
    if (unsolvedConstraint == NULL) {
      cout << spaces(depth) << "- proof succeeded forall left " << toString(ct) << endl;
      return true;
    }
    LOG(DEBUG5, cout << spaces(depth) << "continuing forall left for case " << toString(unsolvedConstraint) << endl);
  }

  ct = ConstrainedTerm(ct.term, simplify(fastAnd(ct.constraint, unsolvedConstraint)));

  smtDefinedSimplify(ConstrainedTerm(lhs, ct.constraint), crsLeft);
  vector<SmtSearchSolution> solutions = prune(smtSearchRewriteSystem(ct, crsRight));
  vector<ConstrainedTerm> lhsSuccs = solutionsToTerms(solutions);
  for (int i = 0; i < (int)lhsSuccs.size(); ++i) {
    ConstrainedTerm afterStep = pairC(lhsSuccs[i].term, rhs, fastAnd(ct.constraint, lhsSuccs[i].constraint));
    if (!proveSimulationForallLeft(afterStep, true, depth + 1)) {
      cout << spaces(depth) << "! proof failed (" << i << "th successor) forall left " << toString(ct) << endl;
      return false;
    }
  }
  if (lhsSuccs.size() > 0) {
    cout << spaces(depth) << "- proof succeeded forall left " << toString(ct) << endl;
    return true;
  }
  else {
    cout << spaces(depth) << "- proof failed forall left (no successors) " << toString(ct) << endl;
    return false;
  }
}


bool QueryProveSim::proveSimulation(ConstrainedTerm ct, int depth) {
  ct = ct;
  cout << spaces(depth) << "Proving simulation circularity " << toString(ct) << endl;
  bool result = proveSimulationForallLeft(ct, false, depth + 1); //needProgressRight = true;
  if (result) {
    cout << spaces(depth) << "Proof succeeded." << endl;
  }
  else {
    cout << spaces(depth) << "Proof failed." << endl;
  }
  return result;
}

void QueryProveSim::execute() {
  LOG(DEBUG9, cout << "Proving simulation" << endl);
  
  int circCount = (int)circularities.size();
  // expand all defined functions in circularities (twice)
  for (int i = 0; i < circCount; ++i) {
    ConstrainedTerm ct = circularities[i];
    vector<SmtSearchSolution> solutions = prune(smtSearchRewriteSystem(ct, rsDefinedCombined));
    vector<ConstrainedTerm> csols = solutionsToTerms(solutions);
    for (int j = 0; j < (int)csols.size(); ++j) {
      ConstrainedTerm newBase = csols[j];
      solutions = prune(smtSearchRewriteSystem(newBase, rsDefinedCombined));
      vector<ConstrainedTerm> csols2 = solutionsToTerms(solutions);
      for (int k = 0; k < (int)(csols2.size()); ++k) {
        ConstrainedTerm newnewBase = csols2[k];
        cout << "Adding new circ (not necessary to prove) " << toString(newnewBase) << endl;
        circularities.push_back(newnewBase);
      }
      LOG(INFO, cout << "Adding new circ (not necessary to prove) " << toString(newBase) << endl);
      circularities.push_back(newBase);
    }
  }

  // prove all circularities
  int nrAssumed = 0;
  for (int i = 0; i < circCount; ++i) {
    cout << "Proving simulation circularity #" << (i + 1) << endl;
    ConstrainedTerm ct = circularities[i];
    if (assumedCircularities[i]) {
      cout << "Circularity #" << (i + 1) << " was assumed to be true" << endl;
      ++nrAssumed;
    }
    else if (proveSimulation(ct, 0)) {
    //else if(true) {
      cout << "Succeeded in proving circularity #" << (i + 1) << endl;
    }
    else {
      cout << "Failed to prove circularity #" << (i + 1) << endl;
      failedCircularities.push_back(i + 1);
    }
  }

  if (failedCircularities.empty()) {
    cout << "Succeeded in proving ALL circularities";
    if (nrAssumed > 0) {
      cout << " (" << nrAssumed << " were assumed)";
    }
    cout << endl;
  }
  else {
    cout << "Failed to prove the following cirularities:";
    for (const auto &idx : failedCircularities)
      cout << " #" << idx;
    cout << endl;
  }
}
