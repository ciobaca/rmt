#include "queryprovereachability.h"
#include "constrainedterm.h"
#include "parse.h"
#include "fastterm.h"
#include "log.h"
#include "helper.h"
#include "smtunify.h"
#include "search.h"
#include <string>
#include <sstream>
#include <map>
#include <iostream>
#include <cassert>

using namespace std;

string stringFromReason(Reason reason)
{
  switch (reason) {
  case DepthLimit:
    return "depth limit exceeded";
  case BranchingLimit:
    return "branching limit exceeded";
  case Completeness:
    return "could not prove completeness";
  }
  LOG(WARNING, cerr << "Unknown reason");
  return "unknown reason";
}

QueryProveReachability::QueryProveReachability()
{
  context = init_z3_context();
}

Query *QueryProveReachability::create()
{
  return new QueryProveReachability();
}

void QueryProveReachability::parse(std::string &s, int &w)
{
  matchString(s, w, "prove");
  skipWhiteSpace(s, w);
  if (lookAhead(s, w, "[")) {
    matchString(s, w, "[");
    skipWhiteSpace(s, w);
    maxDepth = getNumber(s, w);
    if (maxDepth < 0 || maxDepth > 99999) {
      LOG(ERROR, cerr << "Maximum depth (" << maxDepth << ") must be between 0 and 99999");
      expected("Legal maximum depth", w, s);
    }
    skipWhiteSpace(s, w);
    matchString(s, w, ",");
    skipWhiteSpace(s, w);
    maxBranchingDepth = getNumber(s, w);
    if (maxBranchingDepth < 0 || maxBranchingDepth > 99999) {
      LOG(ERROR, cerr << "Maximum branching depth (" << maxBranchingDepth << ") must be between 0 and 99999");
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

extern map<string, RewriteSystem> rewriteSystems;

void QueryProveReachability::execute()
{
  if (!contains(rewriteSystems, rewriteSystemName)) {
    cout << "Cannot find rewrite system " << rewriteSystemName << endl;
    return;
  }
  RewriteSystem rs = rewriteSystems[rewriteSystemName];

  if (!contains(rewriteSystems, circularitiesRewriteSystemName)) {
    cout << "Cannot find rewrite system " << circularitiesRewriteSystemName << endl;
    return;
  }
  RewriteSystem circ = rewriteSystems[circularitiesRewriteSystemName];

  for (int i = 0; i < (int)circ.size(); ++i) {
    ConstrainedTerm lhs = circ[i].first;
    FastTerm rhs = circ[i].second;
    cout << endl;
    cout << "Proving circularity #" << i + 1 << ":" << endl;
    cout << "--------" << endl;
    prove(lhs, rhs, rs, circ, false);
    cout << "--------" << endl;
    cout << "Circularity #" << i + 1 << (unproven.size() ? " not proved. The following proof obligations failed:" : " proved.") << endl;
    
    for (int i = 0; i < (int)unproven.size(); ++i) {
      cout << "Remaining proof obligation #" << i + 1 << " (reason: " << stringFromReason(unproven[i].reason) << "): " << toString(unproven[i].lhs) << " => " << toString(unproven[i].rhs) << endl;
    }
  }
}

// returns a constraint that describes when
// lhs implies rhs
FastTerm QueryProveReachability::proveByImplication(ConstrainedTerm lhs, FastTerm rhs,
			 RewriteSystem &, RewriteSystem &, int depth)
{
  //  FastTerm unificationConstraint;
  FastSubst subst;

  LOG(DEBUG3, cerr << spaces(depth + 1) << "STEP 1. Does lhs imply rhs?");
  vector<SmtUnifySolution> unifiers = smtUnify(lhs.term, rhs);
  FastTerm result = fastFalse();
  for (uint i = 0; i < unifiers.size(); ++i) {
    FastSubst subst = unifiers[i].subst;
    FastTerm unificationConstraint = unifiers[i].constraint;
    // LOG(DEBUG3, cerr << spaces(depth + 1) << "lhs unifies with rhs (unifier " << (i + 1) << "\/" << unifiers.size() << ")");
    LOG(DEBUG3, cerr << spaces(depth + 1) << "Unification constraint: " << toString(unificationConstraint));
    FastTerm constraint = fastImplies(lhs.constraint, unificationConstraint);
    constraint = simplify(constraint);
    if (z3_sat_check(context, fastNot(constraint)) == Z3_L_FALSE) {
      // the negation of the implication is unsatisfiable,
      // meaning that the implication is valid
      LOG(INFO, cerr << spaces(depth + 1) << "RHS is an instance of LHS, this branch is done." << endl);
      return fastTrue();
    } else {
      LOG(DEBUG3, cerr << spaces(depth + 1) << "Constraint " << toString(constraint) << " is not valid.");
      if (z3_sat_check(context, constraint) != Z3_L_FALSE) { // sat or unknown
	LOG(INFO, cerr << spaces(depth + 1) << "RHS is an instance of LHS in case " <<
	    toString(constraint));
	result = fastOr(result, constraint);
      } else {
	LOG(INFO, cerr << spaces(depth + 1) << "RHS is not an instance of LHS in any case (constraint not satisfiable) " << toString(constraint));
      }
    }
  }
  return result;
}

FastTerm introduceExists(FastTerm constraint, vector<FastVar> vars)
{
  for (uint i = 0; i < vars.size(); ++i) {
    if (occurs(vars[i], constraint)) {
      for (uint j = 0; j < vars.size(); ++j) {
      }
      constraint = fastExists(vars[i], constraint);
    }
  }
  return constraint;
}

// returns a constraint that describes when
// rhs can be reached from lhs by applying circularities
FastTerm QueryProveReachability::proveByCircularities(ConstrainedTerm lhs, FastTerm rhs,
			   RewriteSystem &rs, RewriteSystem &circ, int depth, bool hadProgress,
			   int branchingDepth)
{
  //  cout << spaces(depth) << "PROVE CIRC START" << endl;
  LOG(DEBUG3, cerr << spaces(depth + 1) << "STEP 2. Does lhs rewrite using circularities?");
  LOG(DEBUG3, cerr << spaces(depth + 1) << "LHS = " << toString(lhs));

  FastTerm circularityConstraint = fastFalse();
  if (hadProgress) {
    vector<SmtSearchSolution> solutions;
    
    solutions = prune(smtSearchRewriteSystem(lhs, circ));

    LOG(DEBUG3, cerr << spaces(depth + 1) << "Search resulted in " << solutions.size() << " solutions.");
    
    int newBranchDepth = solutions.size() > 1 ? branchingDepth + 1 : branchingDepth;
    for (uint i = 0; i < solutions.size(); ++i) {
      SmtSearchSolution sol = solutions[i];

      FastTerm newTerm = introduceExists(sol.constraint, uniqueVars(sol.lhs));
      circularityConstraint = simplify(fastOr(newTerm, circularityConstraint));

      prove(ConstrainedTerm(sol.subst.applySubst(sol.rhs),
			    sol.subst.applySubst(sol.constraint)),
	    sol.subst.applySubst(rhs), rs, circ, true, depth + 1, newBranchDepth);
    }
  }
  LOG(INFO, cerr << spaces(depth + 1) << "Circularities apply in case: " << toString(circularityConstraint));
  //  cout << spaces(depth) << "PROVE CIRC END" << endl;
  return circularityConstraint;
}

// returns a constraint that describes when
// rhs can be reached from lhs by applying circularities
FastTerm QueryProveReachability::proveByRewrite(ConstrainedTerm lhs, FastTerm rhs,
		     RewriteSystem &rs, RewriteSystem &circ, int depth, bool, int branchingDepth)
{
  //  cout << spaces(depth) << "PROVE REWRITE START" << endl;
  LOG(DEBUG3, cerr << spaces(depth + 1) << "STEP 3. Does lhs rewrite using trusted rewrite rules?");
  LOG(DEBUG3, cerr << spaces(depth + 1) << "LHS = " << toString(lhs));

  // search for all successors in trusted rewrite system
  FastTerm rewriteConstraint = fastFalse();

  vector<SmtSearchSolution> solutions = prune(smtSearchRewriteSystem(lhs, rs));

  LOG(DEBUG3, cerr << spaces(depth + 1) << "Narrowing results in " << solutions.size() << " solutions.");

  int newBranchDepth = (solutions.size() > 1) ? (branchingDepth + 1) : branchingDepth;
  for (uint i = 0; i < solutions.size(); ++i) {
    SmtSearchSolution sol = solutions[i];
 
   rewriteConstraint = simplify(fastOr(introduceExists(sol.constraint,
							uniqueVars(sol.lhs)),
					       rewriteConstraint));
    prove(ConstrainedTerm(sol.subst.applySubst(sol.rhs),
			  sol.subst.applySubst(sol.constraint)),
	  sol.subst.applySubst(rhs), rs, circ, true, depth + 1, newBranchDepth);
  }

  LOG(INFO, cerr << spaces(depth + 1) << "Rewrite rules apply in case: " << toString(rewriteConstraint));

  //  cout << spaces(depth) << "PROVE REWRITE END" << endl;
  return rewriteConstraint;
}

void QueryProveReachability::prove(ConstrainedTerm lhs, FastTerm rhs,
				   RewriteSystem &rs, RewriteSystem &circ, bool hadProgress,
				   int depth, int branchingDepth)
{
  //  cout << spaces(depth) << "PROVE START" << endl;
  if (depth > maxDepth) {
     unproven.push_back(ProofObligation(simplify(lhs), rhs, DepthLimit));
    LOG(WARNING, cerr << spaces(depth) << "(*****) Reached depth" << maxDepth << ", stopping search.");
    return;
  }
  if (branchingDepth > maxBranchingDepth) {
    unproven.push_back(ProofObligation(simplify(lhs), rhs, BranchingLimit));
    LOG(WARNING, cerr << spaces(depth) << "(*****) Reached branch depth " << maxBranchingDepth << ", stopping search.");
    return;
  }

  if (lhs.constraint == 0) {
    lhs.constraint = fastTrue();
  }
  lhs = simplify(lhs);
  ConstrainedTerm initialLhs = lhs;

  LOG(INFO, cerr << spaces(depth) << "Trying to prove " << toString(lhs) << " => " << toString(rhs));

  cout << spaces(depth) << "PROVING " << toString(lhs)  << " => " << toString(rhs) << endl;
  FastTerm implicationConstraint = proveByImplication(lhs, rhs, rs, circ, depth);
  ConstrainedTerm implLhs = lhs;
  implLhs.constraint = simplify(fastAnd(implLhs.constraint, implicationConstraint));
  cout << spaces(depth) << "  IMPL if " << toString(implicationConstraint) << endl;
  lhs.constraint = fastAnd(lhs.constraint, fastNot(implicationConstraint));
  lhs = simplify(lhs);

  FastTerm circularityConstraint = proveByCircularities(lhs, rhs, rs, circ, depth, hadProgress, branchingDepth);
  ConstrainedTerm circLhs = lhs;
  circLhs.constraint = simplify(fastAnd(circLhs.constraint, circularityConstraint));
  cout << spaces(depth) << "  CIRC if " << toString(circularityConstraint) << endl;
  lhs.constraint = fastAnd(lhs.constraint, fastNot(circularityConstraint));
  lhs = simplify(lhs);

  FastTerm rewriteConstraint = proveByRewrite(lhs, rhs, rs, circ, depth, hadProgress, branchingDepth);
  ConstrainedTerm rewrLhs = lhs;
  rewrLhs.constraint = simplify(fastAnd(rewrLhs.constraint, circularityConstraint));
  cout << spaces(depth) << "  REWR if " << toString(rewriteConstraint) << endl;
  lhs.constraint = fastAnd(lhs.constraint, fastNot(rewriteConstraint));
  lhs = simplify(lhs);

  if (z3_sat_check(context, lhs.constraint) != Z3_L_FALSE) {
    cout << spaces(depth) << "! Remaining proof obligation:" << toString(lhs) << " => " << toString(rhs) << endl;
    unproven.push_back(ProofObligation(lhs, rhs, Completeness));

    cout << spaces(depth) << "* Assuming that " << toString(initialLhs) << " => " << toString(rhs) << endl;
  } else {
    cout << spaces(depth) << "OKAY" << endl;
  }
  //  cout << spaces(depth) << "PROVE END" << endl;
}
