#include "queryunify.h"
#include "parse.h"
#include "factories.h"
#include <string>
#include <map>
#include <iostream>
#include "factories.h"

using namespace std;

QueryUnify::QueryUnify()
  : ct(0, 0)
{
}
  
Query *QueryUnify::create()
{
  return new QueryUnify();
}
  
void QueryUnify::parse(std::string &s, int &w)
{
  matchString(s, w, "smt-unify");
  skipWhiteSpace(s, w);
  ct = parseConstrainedTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, "and");
  skipWhiteSpace(s, w);
  term = parseTerm(s, w);
  skipWhiteSpace(s, w);
  matchString(s, w, ";");
}

void QueryUnify::execute()
{
  Substitution subst;
  Term *constraint = 0;
  cout << "Unifying " << ct.toString() << " and " << term->toString() << endl;
  if (term->smtUnifyWith(ct.term, ct.constraint,
			       subst, constraint)) {
    cout << "Unification results:" << endl;
    cout << "Substitution: " << subst.toString() << endl;
    cout << "Constraint: " << constraint->toString() << endl;
    if (existsRewriteSystem("simplifications")) {
      RewriteSystem rs = getRewriteSystem("simplifications");
      constraint = constraint->normalize(rs);
    }
    cout << "Simplified constraint: " << constraint->toString() << endl;
  } else {
    cout << "No unification." << endl;
  }
}
