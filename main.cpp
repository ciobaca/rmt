#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <vector>
#include <sstream>
#include <algorithm>
#include <getopt.h>

#include "helper.h"
#include "term.h"
#include "parse.h"
#include "factories.h"
#include "sort.h"
#include "z3driver.h"
#include "solution.h"
#include "search.h"
#include "constrainedterm.h"
#include "query.h"
#include "funterm.h"
#include "query.h"

static int sober_flag;

string sober(string s) 
{
  return sober_flag ? "" : s;
}

void logmgu(string s, Term *a, Term *b, Substitution &subst)
{
#ifdef LOGMGU
  cerr << s << " " << a->toString() << " and " << b->toString() << endl;
  cerr << "subst = ";
  for (Substitution::iterator it = subst.begin(); it != subst.end(); ++it) {
    cerr << it->first->name << "->" << it->second->toString() << "; ";
  }
  cerr << endl;
#endif
}

void outputRewrite(RewriteSystem &rewriteSystem)
{
  for (int i = 0; i < len(rewriteSystem); ++i) {
    pair<Term *, Term *> rewriteRule = rewriteSystem[i];
    cout << rewriteRule.first->toString() << " " << rewriteRule.second->toString() << endl;
  }
}

void createSubsort(vector<string> &identifiersLeft,
		   vector<string> &identifiersRight)
{
  cout << "Declaring the sorts: ";
  for (int i = 0; i < identifiersLeft.size(); ++i) {
    cout << identifiersLeft[i] << " ";
  }
  cout << "as subsorts of ";
  for (int i = 0; i < identifiersRight.size(); ++i) {
    cout << identifiersRight[i] << " ";
  }
  cout << endl;
  for (int i = 0; i < identifiersLeft.size(); ++i) {
    for (int j = 0; j < identifiersRight.size(); ++j) {
      Sort *left = getSort(identifiersLeft[i]);
      Sort *right = getSort(identifiersRight[j]);
      assert(left);
      assert(right);
      right->addSubSort(left); 
    }
  }
}

void parseSorts(string &s, int &w)
{
  skipWhiteSpace(s, w);
  matchString(s, w, "sorts");
  while (w < len(s)) {
    skipWhiteSpace(s, w);
    string sortName = getIdentifier(s, w);
    string sortInterpretation;
    bool hasInterpretation = false;
    skipWhiteSpace(s, w);
    if (w < len(s) && s[w] == '/') {
      match(s, w, '/');
      skipWhiteSpace(s, w);
      sortInterpretation = getQuotedString(s, w);
      hasInterpretation = true;
      skipWhiteSpace(s, w);
    }
    if (w >= len(s) || (s[w] != ',' && s[w] != ';')) {
      expected("more sort symbols", w, s);
    }
    if (sortExists(sortName)) {
      parseError("sort already exists", w, s);
    }
    if (hasInterpretation) {
      createInterpretedSort(sortName, sortInterpretation);
    } else {
      createUninterpretedSort(sortName);
    }
    if (s[w] == ',') {
      match(s, w, ',');
      continue;
    } else {
      match(s, w, ';');
      break;
    }
  }
}

void parseSubsort(string &s, int &w)
{
  skipWhiteSpace(s, w);
  matchString(s, w, "subsort");
  skipWhiteSpace(s, w);
  vector<string> identifiersLeft;
  if (!lookAheadIdentifier(s, w)) {
    expected("identifier", w, s);
  } else {
    do {
      string id = getIdentifier(s, w);
      if (!sortExists(id)) {
	parseError("sort does not exist", w, s);
      }
      identifiersLeft.push_back(id);
      skipWhiteSpace(s, w);
    } while (lookAheadIdentifier(s, w));
  }
  match(s, w, '<');
  skipWhiteSpace(s, w);
  vector<string> identifiersRight;
  if (!lookAheadIdentifier(s, w)) {
    expected("identifier", w, s);
  } else {
    do {
      string id = getIdentifier(s, w);
      if (!sortExists(id)) {
	parseError("sort does not exist", w, s);
      }
      identifiersRight.push_back(id);
      skipWhiteSpace(s, w);
    } while (lookAheadIdentifier(s, w));
  }
  match(s, w, ';');
  createSubsort(identifiersLeft, identifiersRight);
}

void parseSubsorts(string &s, int &w)
{
  skipWhiteSpace(s, w);
  while (lookAhead(s, w, "subsort")) {
    parseSubsort(s, w);
    skipWhiteSpace(s, w);
  }
}

void parseFunctions(string &s, int &w)
{
  skipWhiteSpace(s, w);
  matchString(s, w, "signature");
  while (w < len(s)) {
    skipWhiteSpace(s, w);
    string f = getIdentifier(s, w);
    skipWhiteSpace(s, w);
    match(s, w, ':');
    skipWhiteSpace(s, w);
    vector<Sort *> arguments;
    while (lookAheadIdentifier(s, w)) {
      string id = getIdentifier(s, w);
      if (!sortExists(id)) {
	parseError("(while parsing function symbol arguments) sort does not exist", w, s);
      }
      skipWhiteSpace(s, w);
      arguments.push_back(getSort(id));
    }
    matchString(s, w, "->");
    skipWhiteSpace(s, w);
    string id = getIdentifier(s, w);
    if (!sortExists(id)) {
      fprintf(stderr, "SORT: %s\n", id.c_str());
      parseError("(while parsing function symbol result) sort does not exist", w, s);
    }
    Sort *result = getSort(id);
    string interpretation;
    bool hasInterpretation = false;
    skipWhiteSpace(s, w);
    if (lookAhead(s, w, "/")) {
      matchString(s, w, "/");
      skipWhiteSpace(s, w);
      interpretation = getQuotedString(s, w);
      hasInterpretation = true;
    }
    skipWhiteSpace(s, w);
    if (hasInterpretation) {
      createInterpretedFunction(f, arguments, result, interpretation);
    } else {
      createUninterpretedFunction(f, arguments, result);
    }
    if (w >= len(s) || (s[w] != ',' && s[w] != ';')) {
      expected("more function symbols", w, s);
    }
    if (s[w] == ',') {
      match(s, w, ',');
      continue;
    } else {
      match(s, w, ';');
      break;
    }
  }
}

void parseVariables(string &s, int &w)
{
  skipWhiteSpace(s, w);
  matchString(s, w, "variables");
  while (w < len(s)) {
    skipWhiteSpace(s, w);
    string n = getIdentifier(s, w);
    if (variableExists(n)) {
      parseError("variable already exists", w, s);
    }
    skipWhiteSpace(s, w);
    matchString(s, w, ":");
    skipWhiteSpace(s, w);
    string id = getIdentifier(s, w);
    if (!sortExists(id)) {
      parseError("(while parsing variables) sort does not exist", w, s);
    }
    Sort *sort = getSort(id);
    createVariable(n, sort);
    if (w >= len(s) || (s[w] != ',' && s[w] != ';')) {
      expected("more variables", w, s);
    }
    skipWhiteSpace(s, w);
    if (s[w] == ',') {
      match(s, w, ',');
      continue;
    } else {
      match(s, w, ';');
      break;
    }
  }
}

string parseRewriteSystem(string &s, int &w, RewriteSystem &rewrite)
{
  skipWhiteSpace(s, w);
  matchString(s, w, "rewrite-system");
  skipWhiteSpace(s, w);
  string name = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  while (w < len(s)) {
    skipWhiteSpace(s, w);
    Term *t = parseTerm(s, w);
    skipWhiteSpace(s, w);
    matchString(s, w, "=>");
    skipWhiteSpace(s, w);
    Term *tp = parseTerm(s, w);
    fprintf(stderr, "Parsed rewrite rule: %s => %s\n", t->toString().c_str(), tp->toString().c_str());
    rewrite.addRule(t, tp);
    skipWhiteSpace(s, w);
    if (w >= len(s) || (s[w] != ',' && s[w] != ';')) {
      expected("more rewrite rules", w, s);
    }
    if (s[w] == ',') {
      match(s, w, ',');
      continue;
    } else {
      match(s, w, ';');
      break;
    }
  }
  return name;
}

// Term *parseAbstract(string &s, int &w)
// {
//   skipWhiteSpace(s, w);
//   matchString(s, w, "abstract");
//   skipWhiteSpace(s, w);
//   Term *result = parseTerm(s, w);
//   match(s, w, ';');
//   skipWhiteSpace(s, w);
//   return result;
// }

// Term *parseRewriteSearch(string &s, int &w)
// {
//   skipWhiteSpace(s, w);
//   matchString(s, w, "rewrite-search");
//   skipWhiteSpace(s, w);
//   Term *result = parseTerm(s, w);
//   match(s, w, ';');
//   skipWhiteSpace(s, w);
//   return result;
// }

// Term *parseNarrowSearch(string &s, int &w)
// {
//   skipWhiteSpace(s, w);
//   matchString(s, w, "narrow-search");
//   skipWhiteSpace(s, w);
//   Term *result = parseTerm(s, w);
//   match(s, w, ';');
//   skipWhiteSpace(s, w);
//   return result;
// }

// Term *parseSmtNarrowSearch(string &s, int &w)
// {
//   skipWhiteSpace(s, w);
//   matchString(s, w, "smt-narrow-search");
//   skipWhiteSpace(s, w);
//   Term *result = parseTerm(s, w);
//   match(s, w, ';');
//   skipWhiteSpace(s, w);
//   return result;
// }

string spaces(int tabs)
{
  ostringstream oss;
  for (int i = 0; i < tabs; ++i) {
    oss << "    ";
  }
  return oss.str();
}

void prove(ConstrainedTerm lhs, ConstrainedTerm rhs,
	   RewriteSystem &rs, RewriteSystem &circ, bool hadProgress, int tabs)
{
  if (tabs > 4) {
    cerr << spaces(tabs) << "Reached depth 4, stopping." << endl;
    return;
  }

  Term *circularityConstraint;

  //  cerr << spaces(tabs) << "SEARCH FOR " << lhs.toString() << endl;
  if (existsRewriteSystem("simplifications")) {
    RewriteSystem rs = getRewriteSystem("simplifications");
    lhs = lhs.normalize(rs);
    //    cerr << spaces(tabs) << "OR SIMPLY " << lhs.toString() << endl;
  }
  cerr << spaces(tabs) << "SIMPL SEARCH FOR " << lhs.toString() << endl;

  Function *TrueFun = getFunction("true");
  Function *FalseFun = getFunction("false");
  Function *NotFun = getFunction("bnot");
  Function *AndFun = getFunction("band");
  Function *OrFun = getFunction("bor");
  Function *EqualsFun = getFunction("bequals");
  {
    vector<Term *> empty;
    circularityConstraint = getFunTerm(FalseFun, empty);
  }

  Term *initialConstraint = lhs.constraint;

  if (hadProgress) {
    vector<ConstrainedTerm> solutions;
    //    cerr << spaces(tabs) << "Narrow search w/ circ from " << lhs.toString() << endl;
    solutions = lhs.smtNarrowSearch(circ);

    if (solutions.size()) {
      cout << spaces(tabs + 1) << "By circularities (" << solutions.size() << " solutions):" << endl;
    }
    for (int i = 0; i < solutions.size(); ++i) {
      ConstrainedTerm sol = solutions[i];

      vector<Term *> arguments;
      arguments.push_back(((FunTerm *)sol.constraint)->arguments[1]);
      // constraint added during unification with rewrite rule

      arguments.push_back(circularityConstraint);
      // or with previous constraint
      
      circularityConstraint = getFunTerm(OrFun, arguments);

      prove(sol, rhs, rs, circ, true, tabs + 1);
    }
  }

  //  cerr << spaces(tabs) << "Circularities handle cases in which " << circularityConstraint->toString() << endl;

  // circularityConstraint now contains the condition in which the circularities
  // can be applied. We need to use the trusted rewrite system to prove 
  // lhs =>* rhs in the rest of the cases.

  //  cerr << spaces(tabs) << "Proving the other cases by rewriting" << endl;
  {
    // first, negate the condition and add the negation to the constraint
    vector<Term *> arguments;
    arguments.push_back(circularityConstraint);
    circularityConstraint = getFunTerm(NotFun, arguments);

    //    cerr << "Negated constraint is " << circularityConstraint->toString() << endl;
  }
  {
    if (lhs.constraint) {
      vector<Term *> arguments;
      arguments.push_back(lhs.constraint);
      arguments.push_back(circularityConstraint);
      lhs.constraint = getFunTerm(AndFun, arguments);
    } else {
      lhs.constraint = circularityConstraint;
    }
    //    cerr << "New constraint is " << lhs.constraint->toString() << endl;
  }

  // search for all successors in trusted rewrite system
  {
    vector<ConstrainedTerm> solutions;
    
    //    cerr << spaces(tabs) << "Searching from " << lhs.toString() << "." << endl;
    //    cerr << spaces(tabs) << "Narrow search w/ trusted from " << lhs.toString() << endl;
    solutions = lhs.smtNarrowSearch(rs);

    //    cerr << spaces(tabs) << "Found " << solutions.size() << " solutions." << endl;
    if (solutions.size()) {
      cout << spaces(tabs + 1) << "By trusted rewrites (" << solutions.size() << " solutions):" << endl;
    }
    for (int i = 0; i < solutions.size(); ++i) {
      ConstrainedTerm sol = solutions[i];
      //      cerr << "Solution #" << i << " is " << sol.toString() << endl;
      prove(sol, rhs, rs, circ, true, tabs + 1);
    } 
  }
}

int VERBOSITY;

int main(int argc, char **argv)
{
  static struct option long_options[] = {
    /* These options set a flag. */
    {"sober", no_argument,       &sober_flag, 1},
    /* These options don't set a flag.
       We distinguish them by their indices. */
    {"verbosity",  required_argument, 0, 'v'},
    {0, 0, 0, 0}
  };

  VERBOSITY = 0;
  while (1) {
    int option_index;
    int c = getopt_long(argc, argv, "v:", long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch (c) {
    case 'v':
      VERBOSITY = atol(optarg);
      break;
    }
  }

  string s;
  int w = 0;

  while (!cin.eof()) {
    string tmp;
    getline(cin, tmp);
    s += tmp;
    s += "\n";
  }

  parseSorts(s, w);
  parseSubsorts(s, w);
  parseFunctions(s, w);
  parseVariables(s, w);
  skipWhiteSpace(s, w);
  int rsCount = 0;
  if (lookAhead(s, w, "rewrite-system")) {
    while (lookAhead(s, w, "rewrite-system")) {
      RewriteSystem rewrite;
      string name = parseRewriteSystem(s, w, rewrite);
      putRewriteSystem(name, rewrite);
      skipWhiteSpace(s, w);
    }
  } else {
    expected("rewrite-system", w, s);
  }

  skipWhiteSpace(s, w);
  Query *query;
  do {
    query = Query::lookAheadQuery(s, w);
    if (!query) {
      break;
    }
    query->parse(s, w);
    skipWhiteSpace(s, w);
    query->execute();
  } while (1);
}
