#include "querysearch.h"
#include "parse.h"
#include "log.h"
#include "helper.h"
#include "rewritesystem.h"
#include "search.h"
#include <string>
#include <map>
#include <iostream>

using namespace std;

QuerySearch::QuerySearch()
  : ct(0, 0)
{
}

Query *QuerySearch::create()
{
  return new QuerySearch();
}
  
void QuerySearch::parse(std::string &s, int &w)
{
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
  ct = parseConstrainedTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");
}

extern map<string, RewriteSystem> rewriteSystems;

void QuerySearch::execute()
{
  if (!contains(rewriteSystems, rewriteSystemName)) {
    cout << "Cannot find (constrained) rewrite system " << rewriteSystemName << endl;
    return;
  }
  RewriteSystem rs = rewriteSystems[rewriteSystemName];

  LOG(DEBUG3, cerr << "Narrowing search from " << toStringCT(ct) << ".");
  vector<ConstrainedTerm> solutions = search(ct, rs, minDepth, maxDepth);
  cout << "Success: " << solutions.size() << " solutions." << endl;
  for (int i = 0; i < (int)solutions.size(); ++i) {
    cout << "Solution #" << i + 1 << ":" << endl;
    // TODO: simplify constrained term
    cout << toStringCT(solutions[i]) << endl;
  }
}
