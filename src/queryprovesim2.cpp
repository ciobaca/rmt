#include "queryprovesim2.h"
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

QueryProveSim2::QueryProveSim2() {
}

Query *QueryProveSim2::create() {
  return new QueryProveSim2();
}

void QueryProveSim2::parse(std::string &s, int &w) {
  matchString(s, w, "show-simulation2");
  skipWhiteSpace(s, w);

  /* defaults */
  boundL = boundR = 100;

  matchString(s, w, "in");
  crsLeft = parseCRSfromName(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, "and");
  crsRight = parseCRSfromName(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ":");
  skipWhiteSpace(s, w);

  do {
    ConstrainedPair ctp = parseConstrainedPair(s, w);
    circularities.push_back(ctp);
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
    ConstrainedPair ctp = parseConstrainedPair(s, w);
    base.push_back(ctp);
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

Term *QueryProveSim2::getBaseConstraint(ConstrainedPair PQ, vector<ConstrainedPair> &base, bool requireProgress) {

  return bTrue();
}

Term *QueryProveSim2::prove(ConstrainedPair PQ, int depth) {
  if (depth >= boundL) {
    cout << spaces(depth) << "! proof failed (exceeded maximum depth) forall left " << PQ.toString() << endl;
    return bFalse();
  }
  return bTrue();
}

void QueryProveSim2::execute() {
  cout << "Not implemented yet!" << endl;
}
