#include "smallqueries.h"
#include "fastterm.h"
#include "parse.h"
#include "abstract.h"
#include "helper.h"
#include "log.h"
#include "search.h"
#include <iostream>
#include <map>
#include <vector>
#include "rewritesystem.h"

using namespace std;

extern map<string, RewriteSystem> rewriteSystems;

void processAbstract(string &s, int &w)
{
  skipWhiteSpace(s, w);
  matchString(s, w, "abstract");
  skipWhiteSpace(s, w);
  FastTerm term = parseTerm(s, w);
  skipWhiteSpace(s, w);
  match(s, w, ';');
  skipWhiteSpace(s, w);

  FastTerm resultTerm;
  FastSubst resultSubst;
  abstractTerm(term, resultTerm, resultSubst);
  cout << "Abstracting          " << toString(term) << ":" << endl;
  cout << "Abstract term     =  " << toString(resultTerm) << endl;
  cout << "Abstracting subst =  " << toString(resultSubst) << endl;
}

void processSearch(string &s, int &w)
{
  std::string rewriteSystemName;

  int minDepth;
  int maxDepth;

  matchString(s, w, "search");
  skipWhiteSpace(s, w);
  if (lookAhead(s, w, "[")) {
    matchString(s, w, "[");
    skipWhiteSpace(s, w);
    minDepth = getNumber(s, w);
    skipWhiteSpace(s, w);
    matchString(s, w, ",");
    skipWhiteSpace(s, w);
    maxDepth = getNumber(s, w);
    skipWhiteSpace(s, w);
    matchString(s, w, "]");
    skipWhiteSpace(s, w);
  } else {
    minDepth = maxDepth = 1;
  }
  skipWhiteSpace(s, w);
  matchString(s, w, "in");
  skipWhiteSpace(s, w);
  rewriteSystemName = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ":");
  skipWhiteSpace(s, w);
  ConstrainedTerm ct = parseConstrainedTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");

  if (!contains(rewriteSystems, rewriteSystemName)) {
    cout << "Cannot find (constrained) rewrite system " << rewriteSystemName << endl;
    return;
  }
  RewriteSystem rs = rewriteSystems[rewriteSystemName];

  LOG(DEBUG3, cerr << "Narrowing search from " << toString(ct) << ".");
  vector<ConstrainedTerm> solutions = search(ct, rs, minDepth, maxDepth);
  cout << "Success: " << solutions.size() << " solutions." << endl;
  for (int i = 0; i < (int)solutions.size(); ++i) {
    cout << "Solution #" << i + 1 << ":" << endl;
    // TODO: simplify constrained term
    cout << toString(solutions[i]) << endl;
  }
}
