#include "querysmtnarrowsearch.h"
#include "parse.h"
#include "factories.h"
#include <string>
#include <map>
#include <iostream>

using namespace std;

QuerySmtNarrowSearch::QuerySmtNarrowSearch()
{
}
  
Query *QuerySmtNarrowSearch::create()
{
  return new QuerySmtNarrowSearch();
}
  
void QuerySmtNarrowSearch::parse(std::string &s, int &w)
{
  matchString(s, w, "smt-narrow-search");

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

void QuerySmtNarrowSearch::execute()
{
  RewriteSystem &rs = getRewriteSystem(rewriteSystemName);

  vector<ConstrainedTerm> solutions = ct.smtNarrowSearch(rs);
  cout << solutions.size() << " solutions." << endl;
  for (int i = 0; i < solutions.size(); ++i) {
    cout << "Solution #" << i + 1 << ":" << endl;
    cout << solutions[i].toString() << endl;
  }
}
