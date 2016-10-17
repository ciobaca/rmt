#include "querysmtprove.h"
#include "constrainedterm.h"
#include "parse.h"
#include "factories.h"
#include "funterm.h"
#include "z3driver.h"
#include "log.h"
#include <string>
#include <sstream>
#include <map>
#include <iostream>
#include <cassert>

using namespace std;

QuerySmtProve::QuerySmtProve()
{
}
  
Query *QuerySmtProve::create()
{
  return new QuerySmtProve();
}

void QuerySmtProve::parse(std::string &s, int &w)
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
  
void QuerySmtProve::execute()
{
  RewriteSystem &rs = getRewriteSystem(rewriteSystemName);
  CRewriteSystem &circ = getCRewriteSystem(circularitiesRewriteSystemName);

  for (int i = 0; i < circ.size(); ++i) {
    ConstrainedTerm lhs = circ[i].first;
    Term *rhs = circ[i].second;
    Log(INFO) << "Proving circularity #" << i + 1 << ":" << endl;
    Log(INFO) << lhs.toString() << " =>* " << rhs->toString() << endl << endl;
    
    prove(lhs, rhs, rs, circ, false);
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
Term *QuerySmtProve::proveByImplication(ConstrainedTerm lhs, Term *rhs,
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
Term *QuerySmtProve::proveByCircularities(ConstrainedTerm lhs, Term *rhs,
			   RewriteSystem &rs, CRewriteSystem &circ, int depth, bool hadProgress,
			   int branchingDepth)
{
  Log(DEBUG) << spaces(depth + 1) << "STEP 2. Does lhs rewrite using circularities?" << endl;
  Log(DEBUG) << spaces(depth + 1) << "LHS = " << lhs.toString() << endl;

  Term *circularityConstraint = bFalse();
  if (hadProgress) {
    vector<ConstrainedTerm> solutions;

    assert(circ);
    solutions = lhs.smtNarrowSearch(circ);

    Log(DEBUG) << spaces(depth + 1) << "Narrowing results in " << solutions.size() << " solutions." << endl;

    int newBranchDepth = solutions.size() > 1 ? branchingDepth + 1 : branchingDepth;
    for (int i = 0; i < solutions.size(); ++i) {
      ConstrainedTerm sol = solutions[i];
      
      circularityConstraint = simplifyConstraint(bOr(sol.constraint, circularityConstraint));
      
      prove(sol, rhs, rs, circ, true, depth + 1, newBranchDepth);
    }
  }
  Log(INFO) << spaces(depth + 1) << "Circularities apply in case: " << circularityConstraint->toString() << endl;
  return circularityConstraint;
}

// returns a constraint that describes when
// rhs can be reached from lhs by applying circularities
Term *QuerySmtProve::proveByRewrite(ConstrainedTerm lhs, Term *rhs,
		     RewriteSystem &rs, CRewriteSystem &circ, int depth, bool hadProgress, int branchingDepth)
{
  Log(DEBUG) << spaces(depth + 1) << "STEP 3. Does lhs rewrite using trusted rewrite rules?" << endl;
  Log(DEBUG) << spaces(depth + 1) << "LHS = " << lhs.toString() << endl;

  // search for all successors in trusted rewrite system
  Term *rewriteConstraint = bFalse();

  vector<ConstrainedTerm> solutions;

  solutions = lhs.smtNarrowSearch(rs);

  Log(DEBUG) << spaces(depth + 1) << "Narrowing results in " << solutions.size() << " solutions." << endl;

  int newBranchDepth = (solutions.size() > 1) ? (branchingDepth + 1) : branchingDepth;
  for (int i = 0; i < solutions.size(); ++i) {
    ConstrainedTerm sol = solutions[i];
    
    rewriteConstraint = simplifyConstraint(bOr(sol.constraint, rewriteConstraint));
    prove(sol, rhs, rs, circ, true, depth + 1, newBranchDepth);
  }

  Log(INFO) << spaces(depth + 1) << "Rewrite rules apply in case: " << rewriteConstraint->toString() << endl;

  return rewriteConstraint;
}

void QuerySmtProve::prove(ConstrainedTerm lhs, Term *rhs,
	   RewriteSystem &rs, CRewriteSystem &circ, bool hadProgress, int depth, int branchingDepth)
{
  //  if (depth > 4) {
  //    Log(WARNING) << spaces(depth) << "(*****) Reached depth 4, stopping search." << endl;
  //    return;
  //  }
  if (depth > maxDepth) {
    Log(WARNING) << spaces(depth) << "(*****) Reached depth" << maxDepth << ", stopping search." << endl;
    return;
  }
  if (branchingDepth > maxBranchingDepth) {
    Log(WARNING) << spaces(depth) << "(*****) Reached branch depth " << maxBranchingDepth << ", stopping search." << endl;
    return;
  }

  if (lhs.constraint == 0) {
    lhs.constraint = bTrue();
  }

  Log(DEBUG) << spaces(depth) << "SEARCH " << lhs.toString() << endl;
  lhs = simplifyConstrainedTerm(lhs);
  Log(INFO) << spaces(depth) << "search " << lhs.toString() << endl;

  Term *implicationConstraint = proveByImplication(lhs, rhs, rs, circ, depth);
  Log(DEBUG) << spaces(depth) << "IMPL CONSTRAINT: " << simplifyConstraint(implicationConstraint)->toString() << endl;
  lhs.constraint = bAnd(lhs.constraint, bNot(implicationConstraint));

  Term *circularityConstraint = proveByCircularities(lhs, rhs, rs, circ, depth, hadProgress, branchingDepth);
  Log(DEBUG) << spaces(depth) << "CIRC CONSTRAINT: " << simplifyConstraint(circularityConstraint)->toString() << endl;
  lhs.constraint = bAnd(lhs.constraint, bNot(circularityConstraint));

  Term *rewriteConstraint = proveByRewrite(lhs, rhs, rs, circ, depth, hadProgress, branchingDepth);
  Log(DEBUG) << spaces(depth) << "REWR CONSTRAINT: " << simplifyConstraint(rewriteConstraint)->toString() << endl;
  lhs.constraint = bAnd(lhs.constraint, bNot(rewriteConstraint));

  if (isSatisfiable(simplifyConstraint(lhs.constraint)) != unsat) {
    Log(INFO) << spaces(depth + 1) << "(*****) Did not finish proof (there are cases remaining)." << endl;
    Log(INFO) << spaces(depth + 1) << "Remaining LHS: " << simplifyConstrainedTerm(lhs).toString() << endl;
  } else {
    Log(INFO) << spaces(depth + 1) << "(*****) Finished all cases." << endl;
  }
}
