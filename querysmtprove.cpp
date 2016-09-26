#include "querysmtprove.h"
#include "constrainedterm.h"
#include "parse.h"
#include "factories.h"
#include <string>
#include <map>
#include <iostream>

using namespace std;

void prove(ConstrainedTerm lhs, ConstrainedTerm rhs,
	   RewriteSystem &rs, RewriteSystem &circ, bool hadProgress, int tabs = 0);

QuerySmtProve::QuerySmtProve()
{
}
  
Query *QuerySmtProve::create()
{
  return new QuerySmtProve();
}
  
void QuerySmtProve::parse(std::string &s, int &w)
{
  matchString(s, w, "smt-prove");
  skipWhiteSpace(s, w);
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
  RewriteSystem &circ = getRewriteSystem(circularitiesRewriteSystemName);

  for (int i = 0; i < circ.size(); ++i) {
    Term *lhs = circ[i].first;
    Term *rhs = circ[i].second;
    cout << endl << endl << "Proving circularity #" << (i + 1) << ":" << endl;
    cout << lhs->toString() << "=>*" << rhs->toString() << endl << endl;
    
    ConstrainedTerm clhs;
    clhs.term = lhs;
    clhs.constraint = 0;
    
    ConstrainedTerm crhs;
    crhs.term = rhs;
    crhs.constraint = 0;
    
    prove(clhs, crhs, rs, circ, false);
  }
}
