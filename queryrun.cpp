#include "queryrun.h"
#include "parse.h"
#include "factories.h"
#include "z3driver.h"
#include <iostream>
#include <string>
#include <map>
#include <cassert>

using namespace std;

QueryRun::QueryRun()
{
}

Query *QueryRun::create()
{
  return new QueryRun();
}
  
void QueryRun::parse(std::string &s, int &w)
{
  //  cerr << "here parse" << endl;
  matchString(s, w, "run");
  skipWhiteSpace(s, w);
  matchString(s, w, "in");
  skipWhiteSpace(s, w);
  rewriteSystemName = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ":");
  skipWhiteSpace(s, w);
  term = parseTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");
}

void QueryRun::execute()
{
  //  cerr << "here execute" << endl;
  //  virtual Term *rewriteOneStep(RewriteSystem &, Substitution &how) = 0;
  RewriteSystem &rs = getRewriteSystem(rewriteSystemName);
  Term *oldTerm;
  Term *newTerm = term;
  Substitution subst;
  const int maxSteps = 10;
  int steps = 0;

  do {
    oldTerm = newTerm;
    cout << "Step " << steps << ": " << oldTerm->toString() << endl;
    if (steps == maxSteps) {
      break;
    }
    newTerm = oldTerm->rewriteOneStep(rs, subst);
    steps++;
  } while (newTerm != oldTerm);
  if (newTerm == oldTerm) {
    cout << "Reached normal form in " << steps << " steps." << endl;
  } else {
    cout << "Reached bound on number of rewrites (" << maxSteps << ")" << endl;
  }
}
