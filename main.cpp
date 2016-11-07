/*
Entry point of RMT tool.

(C) 2016 Stefan Ciobaca
stefan.ciobaca@gmail.com
Faculty of Computer Science
"Alexandru Ioan Cuza" University
Iasi, Romania
http:://profs.info.uaic.ro/~stefan.ciobaca/rmt/
 */
#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <vector>
#include <sstream>
#include <algorithm>
#include "log.h"

#include "helper.h"
#include "term.h"
#include "parse.h"
#include "factories.h"
#include "sort.h"
#include "z3driver.h"
#include "search.h"
#include "constrainedterm.h"
#include "query.h"
#include "funterm.h"
#include "query.h"
#include "constrainedrewrite.h"
#include <getopt.h>

using namespace std;

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
  Log log(INFO);
  log << "Declaring the sorts: ";
  for (int i = 0; i < identifiersLeft.size(); ++i) {
    log << identifiersLeft[i] << " ";
  }
  log << "as subsorts of ";
  for (int i = 0; i < identifiersRight.size(); ++i) {
    log << identifiersRight[i] << " ";
  }
  log << endl;
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

extern string smt_prelude;

void parseSmtPrelude(string &s, int &w)
{
  skipWhiteSpace(s, w);
  if (lookAhead(s, w, "smt-prelude")) {
    matchString(s, w, "smt-prelude");
    skipWhiteSpace(s, w);
    smt_prelude = getQuotedString(s, w);
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
      Log(ERROR) << "SORT: " << id << endl;
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
    Log(INFO) << "Parsed rewrite rule: " << t->toString() << " => " << tp->toString() << endl;
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

string parseCRewriteSystem(string &s, int &w, CRewriteSystem &crewrite)
{
  skipWhiteSpace(s, w);
  matchString(s, w, "constrained-rewrite-system");
  skipWhiteSpace(s, w);
  string name = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  while (w < len(s)) {
    skipWhiteSpace(s, w);
    ConstrainedTerm t = parseConstrainedTerm(s, w);
    skipWhiteSpace(s, w);
    matchString(s, w, "=>");
    skipWhiteSpace(s, w);
    Term *tp = parseTerm(s, w);
    Log(INFO) << "Parsed rewrite rule: " << t.toString() << " => " << tp->toString() << endl;
    crewrite.addRule(t, tp);
    skipWhiteSpace(s, w);
    if (w >= len(s) || (s[w] != ',' && s[w] != ';')) {
      expected("more constrained rewrite rules", w, s);
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

/*
entry point to the RMT tool
 */
int main(int argc, char **argv)
{
  static struct option long_options[] = {
    {"verbosity",  required_argument, 0,           'v'},
    {0, 0, 0, 0}
  };

  while (1) {
    int option_index;
    int c = getopt_long(argc, argv, "v:", long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch (c) {
    case 'v':
      Log::debug_level = atol(optarg);
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

  parseSmtPrelude(s, w);
  parseSorts(s, w);
  parseSubsorts(s, w);
  parseFunctions(s, w);
  parseVariables(s, w);
  skipWhiteSpace(s, w);

  createBuiltIns();

  if (lookAhead(s, w, "rewrite-system") || lookAhead(s, w, "constrained-rewrite-system")) {
    while (lookAhead(s, w, "rewrite-system") || lookAhead(s, w, "constrained-rewrite-system")) {
      if (lookAhead(s, w, "rewrite-system")) {
	RewriteSystem rewrite;
	string name = parseRewriteSystem(s, w, rewrite);
	putRewriteSystem(name, rewrite);
	skipWhiteSpace(s, w);
      } else {
	assert(lookAhead(s, w, "constrained-rewrite-system"));
	CRewriteSystem crewrite;
	string name = parseCRewriteSystem(s, w, crewrite);
	putCRewriteSystem(name, crewrite);
	skipWhiteSpace(s, w);
      }
    }
  } else {
    expected("rewrite-system or constrained-rewrite-system", w, s);
  }

  Log(INFO) << simplifyConstraint(bAnd(bTrue(), bFalse()))->toString() << endl;

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

  if (w < len(s)) {
    expected("query", w, s);
  }
}
