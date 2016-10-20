#include "querysearch.h"
#include "parse.h"
#include "factories.h"
#include <string>
#include <map>
#include <iostream>
#include "log.h"

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

void QuerySearch::execute()
{
  if (existsRewriteSystem(rewriteSystemName)) {
    RewriteSystem &rs = getRewriteSystem(rewriteSystemName);

    vector<ConstrainedTerm> solutions = ct.smtNarrowSearch(rs, minDepth, maxDepth);
    cout << solutions.size() << " solutions." << endl;
    for (int i = 0; i < solutions.size(); ++i) {
      cout << "Solution #" << i + 1 << ":" << endl;
      cout << simplifyConstrainedTerm(solutions[i]).toString() << endl;
    }
  } else if (existsCRewriteSystem(rewriteSystemName)) {
    CRewriteSystem &crs = getCRewriteSystem(rewriteSystemName);

    vector<ConstrainedTerm> solutions = ct.smtNarrowSearch(crs, minDepth, maxDepth);
    cout << solutions.size() << " solutions." << endl;
    for (int i = 0; i < solutions.size(); ++i) {
      cout << "Solution #" << i + 1 << ":" << endl;
      cout << simplifyConstrainedTerm(solutions[i]).toString() << endl;
    }
  } else {
    Log(ERROR) << "Could not find rewrite system " << rewriteSystemName << " (neigher regular or constrained)" << endl;
  }
}
