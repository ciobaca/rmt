#include "queryprove.h"
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

std::string stringFromReason(Reason reason)
{
  switch (reason) {
  case DepthLimit:
    return "depth limit exceeded";
  case BranchingLimit:
    return "branching limit exceeded";
  case Completeness:
    return "could not prove completeness";
  }
  Log(WARNING) << "Unknown reason" << endl;
  return "unknown reason";
}

Term *introduceExists(Term *constraint, vector<Variable *> vars)
{
  for (int i = 0; i < vars.size(); ++i) {
    if (vars[i]->sort->hasInterpretation) {
      constraint = bExists(vars[i], constraint);
    }
  }
  return constraint;
}

QueryProve::QueryProve()
{
}
  
Query *QueryProve::create()
{
  return new QueryProve();
}

void QueryProve::parse(std::string &s, int &w)
{
  matchString(s, w, "prove");
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
  rewriteSystemName = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ":");
  skipWhiteSpace(s, w);
  circularitiesRewriteSystemName = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");
}
  
void QueryProve::execute()
{
  if (existsRewriteSystem(rewriteSystemName)) {
    RewriteSystem &rs = getRewriteSystem(rewriteSystemName);
    CRewriteSystem &circ = getCRewriteSystem(circularitiesRewriteSystemName);

    for (int i = 0; i < circ.size(); ++i) {
      ConstrainedTerm lhs = circ[i].first;
      Term *rhs = circ[i].second;
      cout << endl;
      cout << "Proving circularity #" << i + 1 << ":" << endl;
      cout << "--------" << endl;
      prove(lhs, rhs, rs, circ, false);
      cout << "--------" << endl;
      cout << "Circularity #" << i + 1 << (unproven.size() ? " not proved. The following proof obligations failed:" : " proved.") << endl;
      for (int i = 0; i < unproven.size(); ++i) {
	cout << "Remaining proof obligation #" << i + 1 << " (reason: " << stringFromReason(unproven[i].reason) << "): " <<
	  unproven[i].lhs.toPrettyString() << " => " << unproven[i].rhs->toPrettyString() << endl;
      }
    }
  } else if (existsCRewriteSystem(rewriteSystemName)) {
    CRewriteSystem &crs = getCRewriteSystem(rewriteSystemName);
    CRewriteSystem &circ = getCRewriteSystem(circularitiesRewriteSystemName);

    for (int i = 0; i < circ.size(); ++i) {
      ConstrainedTerm lhs = circ[i].first;
      Term *rhs = circ[i].second;
      cout << endl;
      cout << "Proving circularity #" << i + 1 << ":" << endl;
      cout << "--------" << endl;
      proveCRS(lhs, rhs, crs, circ, false);
      cout << "--------" << endl;
      cout << "Circularity #" << i + 1 << (unproven.size() ? " not proved. The following proof obligations failed:" : " proved.") << endl;
      for (int i = 0; i < unproven.size(); ++i) {
	cout << "Remaining proof obligation #" << i + 1 << " (reason: " << stringFromReason(unproven[i].reason) << "): " <<
	  unproven[i].lhs.toPrettyString() << " => " << unproven[i].rhs->toPrettyString() << endl;
      }
    }
  } else {
    Log(ERROR) << "Cannot find (constrained) rewrite system " << rewriteSystemName << endl;
  }
}

string spaces(int tabs)
{
  ostringstream oss;
  for (int i = 0; i < tabs; ++i) {
    oss << "    ";
  }
  return oss.str();
}

// returns a constraint that describes when
// lhs implies rhs
Term *QueryProve::proveByImplication(ConstrainedTerm lhs, Term *rhs,
			 RewriteSystem &rs, CRewriteSystem &circ, int depth)
{
  Term *unificationConstraint;
  Substitution subst;

  Log(DEBUG) << spaces(depth + 1) << "STEP 1. Does lhs imply rhs?" << endl;
  if (lhs.term->smtUnifyWith(rhs, 0, subst, unificationConstraint)) {
    Log(DEBUG4) << spaces(depth + 1) << "lhs unifies with rhs" << endl;
    Log(DEBUG4) << spaces(depth + 1) << "Unification constraint: " << unificationConstraint->toString() << endl;
    Term *constraint = bImplies(lhs.constraint, unificationConstraint);
    constraint = simplifyConstraint(constraint);
    if (isSatisfiable(bNot(constraint)) == unsat) {
      // the negation of the implication is unsatisfiable,
      // meaning that the implication is valid
      Log(INFO) << spaces(depth + 1) << "RHS is an instance of LHS, this branch is done." << endl;
      return bTrue();
    } else {
      Log(DEBUG2) << spaces(depth + 1) << "Constraint " << constraint->toString() << " is not valid." << endl;
      if (isSatisfiable(constraint) == sat) {
	Log(INFO) << spaces(depth + 1) << "RHS is an instance of LHS in case " <<
	  constraint->toString() << endl;
	return constraint;
      } else {
	Log(INFO) << spaces(depth + 1) << "RHS is not an instance of LHS in any case (constraint not satisfiable) " <<
	  constraint->toString() << endl;
	return bFalse();
      }
    }
  } else {
    Log(INFO) << spaces(depth + 1) << "RHS is not an instance of LHS in any case (no mgu)." << endl;
    return bFalse();
  }
}

// returns a constraint that describes when
// rhs can be reached from lhs by applying circularities
Term *QueryProve::proveByCircularities(ConstrainedTerm lhs, Term *rhs,
			   RewriteSystem &rs, CRewriteSystem &circ, int depth, bool hadProgress,
			   int branchingDepth)
{
  Log(DEBUG) << spaces(depth + 1) << "STEP 2. Does lhs rewrite using circularities?" << endl;
  Log(DEBUG) << spaces(depth + 1) << "LHS = " << lhs.toString() << endl;

  Term *circularityConstraint = bFalse();
  if (hadProgress) {
    vector<ConstrainedSolution> solutions;

    assert(circ);
    solutions = lhs.smtNarrowSearch(circ);

    Log(DEBUG) << spaces(depth + 1) << "Narrowing results in " << solutions.size() << " solutions." << endl;

    int newBranchDepth = solutions.size() > 1 ? branchingDepth + 1 : branchingDepth;
    for (int i = 0; i < solutions.size(); ++i) {
      ConstrainedSolution sol = solutions[i];
      
      circularityConstraint = simplifyConstraint(bOr(
						     introduceExists(sol.constraint, sol.lhsTerm->uniqueVars()),
						     circularityConstraint));
      
      prove(ConstrainedTerm(sol.term, sol.constraint), rhs, rs, circ, true, depth + 1, newBranchDepth);
    }
  }
  Log(INFO) << spaces(depth + 1) << "Circularities apply in case: " << circularityConstraint->toString() << endl;
  return circularityConstraint;
}

// returns a constraint that describes when
// rhs can be reached from lhs by applying circularities
Term *QueryProve::proveByRewrite(ConstrainedTerm lhs, Term *rhs,
		     RewriteSystem &rs, CRewriteSystem &circ, int depth, bool hadProgress, int branchingDepth)
{
  Log(DEBUG) << spaces(depth + 1) << "STEP 3. Does lhs rewrite using trusted rewrite rules?" << endl;
  Log(DEBUG) << spaces(depth + 1) << "LHS = " << lhs.toString() << endl;

  // search for all successors in trusted rewrite system
  Term *rewriteConstraint = bFalse();

  vector<ConstrainedSolution> solutions;

  solutions = lhs.smtNarrowSearch(rs);

  Log(DEBUG) << spaces(depth + 1) << "Narrowing results in " << solutions.size() << " solutions." << endl;

  int newBranchDepth = (solutions.size() > 1) ? (branchingDepth + 1) : branchingDepth;
  for (int i = 0; i < solutions.size(); ++i) {
    ConstrainedSolution sol = solutions[i];
    
    rewriteConstraint = simplifyConstraint(bOr(
					       introduceExists(sol.constraint, sol.lhsTerm->uniqueVars()),
					       rewriteConstraint));
    prove(ConstrainedTerm(sol.term, sol.constraint), rhs, rs, circ, true, depth + 1, newBranchDepth);
  }

  Log(INFO) << spaces(depth + 1) << "Rewrite rules apply in case: " << rewriteConstraint->toString() << endl;

  return rewriteConstraint;
}

void QueryProve::prove(ConstrainedTerm lhs, Term *rhs,
	   RewriteSystem &rs, CRewriteSystem &circ, bool hadProgress, int depth, int branchingDepth)
{
  if (depth > maxDepth) {
    unproven.push_back(ProofObligation(simplifyConstrainedTerm(lhs), rhs, DepthLimit));
    Log(WARNING) << spaces(depth) << "(*****) Reached depth" << maxDepth << ", stopping search." << endl;
    return;
  }
  if (branchingDepth > maxBranchingDepth) {
    unproven.push_back(ProofObligation(simplifyConstrainedTerm(lhs), rhs, BranchingLimit));
    Log(WARNING) << spaces(depth) << "(*****) Reached branch depth " << maxBranchingDepth << ", stopping search." << endl;
    return;
  }

  if (lhs.constraint == 0) {
    lhs.constraint = bTrue();
  }

  lhs = simplifyConstrainedTerm(lhs);
  ConstrainedTerm initialLhs = lhs;

  Log(INFO) << spaces(depth) << "Trying to prove " << lhs.toString() << " => " << rhs->toString() << endl;

  Term *implicationConstraint = proveByImplication(lhs, rhs, rs, circ, depth);
  ConstrainedTerm implLhs = lhs;
  implLhs.constraint = simplifyConstraint(bAnd(implLhs.constraint, implicationConstraint));
  cout << spaces(depth + 1) << "- " << implLhs.toPrettyString() << " -----> " << rhs->toPrettyString() << endl;
  lhs.constraint = bAnd(lhs.constraint, bNot(implicationConstraint));
  lhs = simplifyConstrainedTerm(lhs);

  Term *circularityConstraint = proveByCircularities(lhs, rhs, rs, circ, depth, hadProgress, branchingDepth);
  ConstrainedTerm circLhs = lhs;
  circLhs.constraint = simplifyConstraint(bAnd(circLhs.constraint, circularityConstraint));
  cout << spaces(depth + 1) << "- " << circLhs.toPrettyString() << " =(C)=> " << rhs->toPrettyString() << endl;
  lhs.constraint = bAnd(lhs.constraint, bNot(circularityConstraint));
  lhs = simplifyConstrainedTerm(lhs);

  Term *rewriteConstraint = proveByRewrite(lhs, rhs, rs, circ, depth, hadProgress, branchingDepth);
  ConstrainedTerm rewrLhs = lhs;
  rewrLhs.constraint = simplifyConstraint(bAnd(rewrLhs.constraint, circularityConstraint));
  cout << spaces(depth + 1) << "- " << lhs.toPrettyString() << " =(R)=> " << rhs->toPrettyString() << endl;
  lhs.constraint = bAnd(lhs.constraint, bNot(rewriteConstraint));
  lhs = simplifyConstrainedTerm(lhs);

  if (isSatisfiable(simplifyConstraint(lhs.constraint)) != unsat) {
    cout << spaces(depth) << "! Could not show that " <<  simplifyConstrainedTerm(lhs).toPrettyString() << " => " << rhs->toPrettyString() << endl;
    unproven.push_back(ProofObligation(simplifyConstrainedTerm(lhs), rhs, Completeness));

    cout << spaces(depth) << "* Assuming that " << initialLhs.toPrettyString() << " => " << rhs->toPrettyString() << endl;
    //    Log(INFO) << spaces(depth + 1) << "(*****) Did not finish proof (there are cases remaining)." << endl;
    //    Log(INFO) << spaces(depth + 1) << "Remaining LHS: " << simplifyConstrainedTerm(lhs).toString() << endl;
  } else {
    cout << spaces(depth) << "* Proved that " << initialLhs.toPrettyString() << " => " << rhs->toPrettyString() << endl;
    //    Log(INFO) << spaces(depth + 1) << "(*****) Finished all cases." << endl;
  }
}


// same algorithm for CRS

// returns a constraint that describes when
// lhs implies rhs
Term *QueryProve::proveByImplicationCRS(ConstrainedTerm lhs, Term *rhs,
			 CRewriteSystem &crs, CRewriteSystem &circ, int depth)
{
  Term *unificationConstraint;
  Substitution subst;

  Log(DEBUG) << spaces(depth + 1) << "STEP 1. Does lhs imply rhs?" << endl;
  if (lhs.term->smtUnifyWith(rhs, 0, subst, unificationConstraint)) {
    Log(DEBUG4) << spaces(depth + 1) << "lhs unifies with rhs" << endl;
    Log(DEBUG4) << spaces(depth + 1) << "Unification constraint: " << unificationConstraint->toString() << endl;
    Term *constraint = bImplies(lhs.constraint, unificationConstraint);
    constraint = simplifyConstraint(constraint);
    if (isSatisfiable(bNot(constraint)) == unsat) {
      // the negation of the implication is unsatisfiable,
      // meaning that the implication is valid
      Log(INFO) << spaces(depth + 1) << "RHS is an instance of LHS, this branch is done." << endl;
      return bTrue();
    } else {
      Log(DEBUG2) << spaces(depth + 1) << "Constraint " << constraint->toString() << " is not valid." << endl;
      if (isSatisfiable(constraint) == sat) {
	Log(INFO) << spaces(depth + 1) << "RHS is an instance of LHS in case " <<
	  constraint->toString() << endl;
	return constraint;
      } else {
	Log(INFO) << spaces(depth + 1) << "RHS is not an instance of LHS in any case (constraint not satisfiable) " <<
	  constraint->toString() << endl;
	return bFalse();
      }
    }
  } else {
    Log(INFO) << spaces(depth + 1) << "RHS is not an instance of LHS in any case (no mgu)." << endl;
    return bFalse();
  }
}

// returns a constraint that describes when
// rhs can be reached from lhs by applying circularities
Term *QueryProve::proveByCircularitiesCRS(ConstrainedTerm lhs, Term *rhs,
			   CRewriteSystem &crs, CRewriteSystem &circ, int depth, bool hadProgress,
			   int branchingDepth)
{
  Log(DEBUG) << spaces(depth + 1) << "STEP 2. Does lhs rewrite using circularities?" << endl;
  Log(DEBUG) << spaces(depth + 1) << "LHS = " << lhs.toString() << endl;

  Term *circularityConstraint = bFalse();
  if (hadProgress) {
    vector<ConstrainedSolution> solutions;

    assert(circ);
    solutions = lhs.smtNarrowSearch(circ);

    Log(DEBUG) << spaces(depth + 1) << "Narrowing results in " << solutions.size() << " solutions." << endl;

    int newBranchDepth = solutions.size() > 1 ? branchingDepth + 1 : branchingDepth;
    for (int i = 0; i < solutions.size(); ++i) {
      ConstrainedSolution sol = solutions[i];

      circularityConstraint = simplifyConstraint(bOr(
						     introduceExists(sol.constraint, sol.lhsTerm->uniqueVars()),
						     circularityConstraint));

      proveCRS(ConstrainedTerm(sol.term, sol.constraint), rhs, crs, circ, true, depth + 1, newBranchDepth);
    }
  }
  Log(INFO) << spaces(depth + 1) << "Circularities apply in case: " << circularityConstraint->toString() << endl;
  return circularityConstraint;
}

// returns a constraint that describes when
// rhs can be reached from lhs by applying circularities
Term *QueryProve::proveByRewriteCRS(ConstrainedTerm lhs, Term *rhs,
		     CRewriteSystem &crs, CRewriteSystem &circ, int depth, bool hadProgress, int branchingDepth)
{
  Log(DEBUG) << spaces(depth + 1) << "STEP 3. Does lhs rewrite using trusted rewrite rules?" << endl;
  Log(DEBUG) << spaces(depth + 1) << "LHS = " << lhs.toString() << endl;

  // search for all successors in trusted rewrite system
  Term *rewriteConstraint = bFalse();

  vector<ConstrainedSolution> solutions = lhs.smtNarrowSearch(crs);

  Log(DEBUG) << spaces(depth + 1) << "Narrowing results in " << solutions.size() << " solutions." << endl;

  int newBranchDepth = (solutions.size() > 1) ? (branchingDepth + 1) : branchingDepth;
  for (int i = 0; i < solutions.size(); ++i) {
    ConstrainedSolution sol = solutions[i];

    rewriteConstraint = simplifyConstraint(bOr(
					       introduceExists(sol.constraint, sol.lhsTerm->uniqueVars()),
					       rewriteConstraint));
    proveCRS(ConstrainedTerm(sol.term, sol.constraint), rhs, crs, circ, true, depth + 1, newBranchDepth);
  }

  Log(INFO) << spaces(depth + 1) << "Rewrite rules apply in case: " << rewriteConstraint->toString() << endl;

  return rewriteConstraint;
}

void QueryProve::proveCRS(ConstrainedTerm lhs, Term *rhs,
			  CRewriteSystem &crs, CRewriteSystem &circ, bool hadProgress, int depth, int branchingDepth
			  )
{
  if (depth > maxDepth) {
    unproven.push_back(ProofObligation(simplifyConstrainedTerm(lhs), rhs, DepthLimit));
    Log(WARNING) << spaces(depth) << "(*****) Reached depth" << maxDepth << ", stopping search." << endl;
    return;
  }
  if (branchingDepth > maxBranchingDepth) {
    unproven.push_back(ProofObligation(simplifyConstrainedTerm(lhs), rhs, BranchingLimit));
    Log(WARNING) << spaces(depth) << "(*****) Reached branch depth " << maxBranchingDepth << ", stopping search." << endl;
    return;
  }

  if (lhs.constraint == 0) {
    lhs.constraint = bTrue();
  }

  lhs = simplifyConstrainedTerm(lhs);
  ConstrainedTerm initialLhs = lhs;

  Log(INFO) << spaces(depth) << "Trying to prove " << lhs.toString() << " => " << rhs->toString() << endl;

  Term *implicationConstraint = proveByImplicationCRS(lhs, rhs, crs, circ, depth);
  ConstrainedTerm implLhs = lhs;
  implLhs.constraint = simplifyConstraint(bAnd(implLhs.constraint, implicationConstraint));
  cout << spaces(depth + 1) << "- " << implLhs.toPrettyString() << " -----> " << rhs->toPrettyString() << endl;
  lhs.constraint = bAnd(lhs.constraint, bNot(implicationConstraint));
  lhs = simplifyConstrainedTerm(lhs);

  Term *circularityConstraint = proveByCircularitiesCRS(lhs, rhs, crs, circ, depth, hadProgress, branchingDepth);
  ConstrainedTerm circLhs = lhs;
  circLhs.constraint = simplifyConstraint(bAnd(circLhs.constraint, circularityConstraint));
  cout << spaces(depth + 1) << "- " << circLhs.toPrettyString() << " =(C)=> " << rhs->toPrettyString() << endl;
  lhs.constraint = bAnd(lhs.constraint, bNot(circularityConstraint));
  lhs = simplifyConstrainedTerm(lhs);

  Term *rewriteConstraint = proveByRewriteCRS(lhs, rhs, crs, circ, depth, hadProgress, branchingDepth);
  ConstrainedTerm rewrLhs = lhs;
  rewrLhs.constraint = simplifyConstraint(bAnd(rewrLhs.constraint, circularityConstraint));
  cout << spaces(depth + 1) << "- " << lhs.toPrettyString() << " =(R)=> " << rhs->toPrettyString() << endl;
  lhs.constraint = bAnd(lhs.constraint, bNot(rewriteConstraint));
  lhs = simplifyConstrainedTerm(lhs);

  if (isSatisfiable(simplifyConstraint(lhs.constraint)) != unsat) {
    cout << spaces(depth) << "! Could not show that " <<  simplifyConstrainedTerm(lhs).toPrettyString() << " => " << rhs->toPrettyString() << endl;
    unproven.push_back(ProofObligation(simplifyConstrainedTerm(lhs), rhs, Completeness));

    cout << spaces(depth) << "* Assuming that " << initialLhs.toPrettyString() << " => " << rhs->toPrettyString() << endl;
  } else {
    cout << spaces(depth) << "* Proved that " << initialLhs.toPrettyString() << " => " << rhs->toPrettyString() << endl;
  }
}
