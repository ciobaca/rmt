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
}

extern map<string, RewriteSystem> rewriteSystems;

void QueryProveSim::execute() {
  cout << "Keep waiting, boy!" << endl;
}
