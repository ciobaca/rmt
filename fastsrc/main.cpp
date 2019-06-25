/*

Entry point of the RMT tool.

(C) 2016 - 2019 Stefan Ciobaca
stefan.ciobaca@gmail.com
Faculty of Computer Science
Alexandru Ioan Cuza University
Iasi, Romania

http://profs.info.uaic.ro/~stefan.ciobaca/
*/
#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include "log.h"
#include "helper.h"
#include "parse.h"
#include "fastterm.h"
#include "smallqueries.h"

// #include "term.h"
// #include "factories.h"
// #include "sort.h"
// #include "z3driver.h"
// #include "search.h"
// #include "constrainedterm.h"
 #include "query.h"
// #include "funterm.h"
 #include "rewritesystem.h"

using namespace std;

map<string, RewriteSystem> rewriteSystems;

// void outputRewriteSystem(RewriteSystem &rewriteSystem)
// {
//   for (int i = 0; i < len(rewriteSystem); ++i) {
//     pair<Term *, Term *> rewriteRule = rewriteSystem[i];
//     cout << rewriteRule.first->toString() << " " << rewriteRule.second->toString() << endl;
//   }
// }

// void parseBuiltins(string &s, int &w)
// {
//   skipWhiteSpace(s, w);
//   if (!lookAhead(s, w, "builtins")) {
//     return;
//   }
//   matchString(s, w, "builtins");
//   while (w < len(s)) {
//     skipWhiteSpace(s, w);
//     string f = getIdentifier(s, w);
//     skipWhiteSpace(s, w);
//     match(s, w, ':');
//     skipWhiteSpace(s, w);
//     vector<Sort *> arguments;
//     while (lookAheadIdentifier(s, w)) {
//       string id = getIdentifier(s, w);
//       if (!existsSort(id)) {
//         parseError("(while parsing builtins) sort does not exist", w, s);
//       }
//       if (!isBuiltinSort(id)) {
//         parseError("(while parsing builtins) sort is not builtin", w, s);
//       }
//       skipWhiteSpace(s, w);
//       arguments.push_back(getSort(id));
//     }
//     matchString(s, w, "->");
//     skipWhiteSpace(s, w);
//     string id = getIdentifier(s, w);
//     if (!existsSort(id)) {
//       LOG(ERROR) << "SORT: " << id << endl;
//       parseError("(while parsing builtins) sort does not exist", w, s);
//     }
//     if (!isBuiltinSort(id)) {
//       parseError("(while parsing builtins) sort is not builtin", w, s);
//     }
//     Sort *result = getSort(id);
//     string interpretation = f;
//     skipWhiteSpace(s, w);

//     createInterpretedFunction(f, arguments, result, createZ3FunctionSymbol(f, arguments, result));
//     if (w >= len(s) || (s[w] != ',' && s[w] != ';')) {
//       expected("more builtin function symbols", w, s);
//     }
//     if (s[w] == ',') {
//       match(s, w, ',');
//       continue;
//     }
//     else {
//       match(s, w, ';');
//       break;
//     }
//   }
// }

// void parseAsserts(string &s, int &w)
// {
//   skipWhiteSpace(s, w);
//   while (lookAhead(s, w, "assert")) {
//     matchString(s, w, "assert");
//     skipWhiteSpace(s, w);
//     Term *term = parseTerm(s, w);
//     if (term->getSort() != getBoolSort()) {
//       parseError("(while parsing asserts) asserts should be of sort Bool.", w, s);
//     }
//     addZ3Assert(term);
//     skipWhiteSpace(s, w);
//     matchString(s, w, ";");
//     skipWhiteSpace(s, w);
//   }
// }

// void parseDefinedFunctions(string &s, int &w)
// {
//   skipWhiteSpace(s, w);
//   while (lookAhead(s, w, "define")) {
//     matchString(s, w, "define");
//     skipWhiteSpace(s, w);

//     string f = getIdentifier(s, w);
//     skipWhiteSpace(s, w);
//     match(s, w, ':');
//     skipWhiteSpace(s, w);
//     vector<Sort *> arguments;
//     while (lookAheadIdentifier(s, w)) {
//       string id = getIdentifier(s, w);
//       if (!existsSort(id)) {
//         parseError("(while parsing defined function) sort does not exist", w, s);
//       }
//       skipWhiteSpace(s, w);
//       arguments.push_back(getSort(id));
//     }
//     matchString(s, w, "->");
//     skipWhiteSpace(s, w);
//     string id = getIdentifier(s, w);
//     if (!existsSort(id)) {
//       LOG(ERROR) << "SORT: " << id << endl;
//       parseError("(while parsing defined function) sort does not exist", w, s);
//     }
//     Sort *result = getSort(id);

//     createUninterpretedFunction(f, arguments, result);

//     ConstrainedRewriteSystem crewrite;
//     skipWhiteSpace(s, w);
//     matchString(s, w, "by");
//     skipWhiteSpace(s, w);
//     while (w < len(s)) {
//       skipWhiteSpace(s, w);
//       ConstrainedTerm t = parseConstrainedTerm(s, w);
//       skipWhiteSpace(s, w);
//       matchString(s, w, "=>");
//       skipWhiteSpace(s, w);
//       Term *tp = parseTerm(s, w);
//       LOG(INFO) << "Parsed rewrite rule for func def: " << t.toString() << " => " << tp->toString() << endl;
//       crewrite.addRule(t, tp);
//       skipWhiteSpace(s, w);
//       if (w >= len(s) || (s[w] != ',' && s[w] != ';')) {
//         expected("more constrained rewrite rules", w, s);
//       }
//       if (s[w] == ',') {
//         match(s, w, ',');
//         continue;
//       }
//       else {
//         match(s, w, ';');
//         break;
//       }
//     }
//     updateDefinedFunction(f, crewrite);

//     skipWhiteSpace(s, w);
//   }
// }

// void addPredefinedSorts()
// {
//   createInterpretedSort("Bool", "Bool");
//   createInterpretedSort("Int", "Int");
// }

void parseSorts(string &s, int &w)
{
  skipWhiteSpace(s, w);
  matchString(s, w, "sorts");
  while (w < len(s)) {
    skipWhiteSpace(s, w);
    string sortName = getIdentifier(s, w);
    if (existsSort(sortName.c_str())) {
      parseError("sort already exists", w, s);
    }
    if (identifierTaken(sortName.c_str())) {
      parseError("identifier already used", w, s);
    }
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
    LOG(DEBUG9, cerr << "Parsed sort " << sortName);
    if (hasInterpretation) {
      parseError("todo: cannot handle interpreted sorts yet", w, s);
      // this is a builtin sort!! .. will be refactored later (TODO)
      // createInterpretedSort(sortName, sortInterpretation);
      // createBuiltinExistsFunction(getSort(sortName));
      // createBuiltinForallFunction(getSort(sortName));
    } else {
      newSort(sortName.c_str());
    }
    if (s[w] == ',') {
      match(s, w, ',');
      continue;
    }
    else {
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
  vector<FastSort> sortsLeft;
  if (!lookAheadIdentifier(s, w)) {
    expected("identifier", w, s);
  } else {
    do {
      string id = getIdentifier(s, w);
      if (!existsSort(id.c_str())) {
        parseError("sort does not exist", w, s);
      }
      sortsLeft.push_back(getSortByName(id.c_str()));
      skipWhiteSpace(s, w);
    } while (lookAheadIdentifier(s, w));
  }
  match(s, w, '<');
  skipWhiteSpace(s, w);
  vector<FastSort> sortsRight;
  if (!lookAheadIdentifier(s, w)) {
    expected("identifier", w, s);
  } else {
    do {
      string id = getIdentifier(s, w);
      if (!existsSort(id.c_str())) {
        parseError("sort does not exist", w, s);
      }
      sortsRight.push_back(getSortByName(id.c_str()));
      skipWhiteSpace(s, w);
    } while (lookAheadIdentifier(s, w));
  }
  match(s, w, ';');
  for (int i = 0; i < (int)sortsLeft.size(); ++i) {
    for (int j = 0; j < (int)sortsRight.size(); ++j) {
      LOG(DEBUG3, cerr << "Adding " << getSortName(sortsLeft[i]) << " as subsort of " << getSortName(sortsRight[j]));
      FastSort left = sortsLeft[i];
      FastSort right = sortsRight[j];
      if (isSubSortTransitive(right, left)) {
	parseError("cannot add subsorting (would create cycle)", w, s);
      }
      if (isBuiltinSort(right)) {
	parseError("cannot add subsorting to builtin sorts", w, s);
      }
      newSubSort(left, right);
    }
  }
}

void parseSubsorts(string &s, int &w)
{
  skipWhiteSpace(s, w);
  while (lookAhead(s, w, "subsort")) {
    parseSubsort(s, w);
    skipWhiteSpace(s, w);
  }
}

// void addPredefinedFunctions()
// {
//   Sort *intSort = getSort("Int");
//   Sort *boolSort = getSort("Bool");
//   assert(intSort);
//   assert(boolSort);

//   {
//     vector<Sort *> arguments;
//     createInterpretedFunction("0", arguments, intSort, "0");
//     createInterpretedFunction("1", arguments, intSort, "1");
//     createInterpretedFunction("2", arguments, intSort, "2");
//     createInterpretedFunction("3", arguments, intSort, "3");
//     createInterpretedFunction("4", arguments, intSort, "4");
//     createInterpretedFunction("5", arguments, intSort, "5");
//     createInterpretedFunction("6", arguments, intSort, "6");
//     createInterpretedFunction("7", arguments, intSort, "7");
//     createInterpretedFunction("8", arguments, intSort, "8");
//     createInterpretedFunction("9", arguments, intSort, "9");
//     createInterpretedFunction("10", arguments, intSort, "10");
//     createInterpretedFunction("11", arguments, intSort, "11");
//     createInterpretedFunction("12", arguments, intSort, "12");
//     createInterpretedFunction("13", arguments, intSort, "13");
//     createInterpretedFunction("14", arguments, intSort, "14");
//     createInterpretedFunction("15", arguments, intSort, "15");

//     arguments.push_back(intSort);
//     arguments.push_back(intSort);
//     createInterpretedFunction("mplus", arguments, intSort, "+");
//     createInterpretedFunction("mminus", arguments, intSort, "-");
//     createInterpretedFunction("mtimes", arguments, intSort, "*");
//     createInterpretedFunction("mdiv", arguments, intSort, "div");
//     createInterpretedFunction("mmod", arguments, intSort, "mod");
//     createInterpretedFunction("mle", arguments, boolSort, "<=");
//     createInterpretedFunction("mge", arguments, boolSort, ">=");
//     createInterpretedFunction("mless", arguments, boolSort, "<");
//     createInterpretedFunction("mgt", arguments, boolSort, ">");
//     createInterpretedFunction("mequals", arguments, boolSort, "=");

//     arguments.push_back(boolSort);
//     std::reverse(arguments.begin(), arguments.end());
//     createInterpretedFunction("iteInt", arguments, boolSort, "ite");
//   }

//   {
//     vector<Sort *> arguments;
//     createInterpretedFunction("true", arguments, boolSort, "true");
//     createInterpretedFunction("false", arguments, boolSort, "false");

//     arguments.push_back(boolSort);
//     createInterpretedFunction("bnot", arguments, boolSort, "not");

//     arguments.push_back(boolSort);
//     createInterpretedFunction("band", arguments, boolSort, "and");
//     createInterpretedFunction("biff", arguments, boolSort, "iff");
//     createInterpretedFunction("bor", arguments, boolSort, "or");
//     createInterpretedFunction("bimplies", arguments, boolSort, "=>");
//     createInterpretedFunction("bequals", arguments, boolSort, "=");
//   }
// }

void parseFunctions(string &s, int &w)
{
  skipWhiteSpace(s, w);
  matchString(s, w, "signature");
  while (w < len(s)) {
    skipWhiteSpace(s, w);
    string f = getIdentifier(s, w);
    if (existsFunc(f.c_str())) {
      parseError("duplicate function symbol", w, s);
    }
    if (identifierTaken(f.c_str())) {
      parseError("identifier already used", w, s);
    }
    skipWhiteSpace(s, w);
    match(s, w, ':');
    skipWhiteSpace(s, w);
    vector<FastSort> arguments;
    while (lookAheadIdentifier(s, w)) {
      string id = getIdentifier(s, w);
      if (!existsSort(id.c_str())) {
        parseError("(while parsing function symbol arguments) sort does not exist", w, s);
      }
      skipWhiteSpace(s, w);
      arguments.push_back(getSortByName(id.c_str()));
    }
    matchString(s, w, "->");
    skipWhiteSpace(s, w);
    string id = getIdentifier(s, w);
    if (!existsSort(id.c_str())) {
      LOG(ERROR, cerr << "SORT: " << id);
      parseError("(while parsing function symbol result) sort does not exist", w, s);
    }
    FastSort result = getSortByName(id.c_str());
    string interpretation;
    bool hasInterpretation = false;
    skipWhiteSpace(s, w);
    if (lookAhead(s, w, "/")) {
      matchString(s, w, "/");
      skipWhiteSpace(s, w);
      if (lookAhead(s, w, "\"")) {
        interpretation = getQuotedString(s, w);
        hasInterpretation = true;
      } else {
        matchString(s, w, "\"");
      }
    }
    skipWhiteSpace(s, w);
    bool isCommutative = false;
    bool isAssociative = false;
    bool hasUnity = false;
    FastFunc unityElement;
    if (lookAhead(s, w, "[")) {
      matchString(s, w, "[");
      skipWhiteSpace(s, w);
      while (w < len(s) && s[w] != ']') {
        switch(s[w]) {
          case 'a': isAssociative = true; ++w; break;
          case 'c': isCommutative = true; ++w; break;
          case 'u': {
	    hasUnity = true;
            matchString(s, w, "u");
            skipWhiteSpace(s, w);
            if (lookAhead(s, w, "(")) {
              matchString(s, w, "(");
              skipWhiteSpace(s, w);
              string id = getIdentifier(s, w);
              unityElement = getFuncByName(id.c_str());
              skipWhiteSpace(s, w);
            } else {
              parseError("(while parsing unity element) '(' is missing", w, s);
            }
            if (lookAhead(s, w, ")")) {
              matchString(s, w, ")");
            } else {
              parseError("(while parsing unity element) ')' is missing", w, s);
            }
          }
        }
        skipWhiteSpace(s, w);
      }
      matchString(s, w, "]");
    }
    if (hasInterpretation) {
      parseError("todo: cannot handle interpreted functions yet", w, s);
      //      createInterpretedFunction(f, arguments, result, interpretation);
    } else {
      if ((isCommutative || isAssociative) && arguments.size() != 2) {
        parseError("Associative or commutative functions must have exactly two arguments", w, s);
      }
      if (!isCommutative && !isAssociative && !hasUnity) {
	newFunc(f.c_str(), result, arguments.size(), &arguments[0]);
      } else if (isCommutative && isAssociative && !hasUnity) {
	for (uint32 i = 0; i < arguments.size(); ++i) {
	  if (arguments[i] != result) {
	    parseError("AC functions must be defined on the same sort as the result sort", w, s);
	  }
	}
	newACFunc(f.c_str(), result);
      } else if (isCommutative && isAssociative && hasUnity) {
	for (uint32 i = 0; i < arguments.size(); ++i) {
	  if (arguments[i] != result) {
	    parseError("AC functions must be defined on the same sort as the result", w, s);
	  }
	}
	if (result != getFuncSort(unityElement)) {
	  parseError("Unity element of ACU function must have the same sort", w, s);
	}
	if (getArity(unityElement) != 0) {
	  parseError("Unity element of ACU function must be nullary", w, s);
	}
	newACUFunc(f.c_str(), unityElement);
      } else {
	parseError("Unsupported combination of a/c/u properties", w, s);
      }
    }
    if (w >= len(s) || (s[w] != ',' && s[w] != ';')) {
      expected("more function symbols", w, s);
    }
    if (s[w] == ',') {
      match(s, w, ',');
      continue;
    }
    else {
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
    if (existsVar(n.c_str())) {
      parseError("variable already exists", w, s);
    }
    if (identifierTaken(n.c_str())) {
      parseError("identifier already used", w, s);
    }
    skipWhiteSpace(s, w);
    matchString(s, w, ":");
    skipWhiteSpace(s, w);
    string id = getIdentifier(s, w);
    if (!existsSort(id.c_str())) {
      parseError("(while parsing variables) sort does not exist", w, s);
    }
    FastSort sort = getSortByName(id.c_str());
    newVar(n.c_str(), sort);
    //    LOG(DEBUG9, "newVar " << n << id);
    skipWhiteSpace(s, w);
    if (w >= len(s) || (s[w] != ',' && s[w] != ';')) {
      expected("more variables", w, s);
    }
    if (s[w] == ',') {
      match(s, w, ',');
      continue;
    }
    else {
      match(s, w, ';');
      break;
    }
  }
}

// void addPredefinedRewriteSystems()
// {
//   return;

//   RewriteSystem rewrite;

//   Sort *boolSort = getSort("Bool");
//   Sort *intSort = getSort("Int");
//   Term *B = getVarTerm(createFreshVariable(boolSort));
//   Term *N = getVarTerm(createFreshVariable(intSort));

//   rewrite.addRule(bNot(bFalse()), bTrue());
//   rewrite.addRule(bNot(bTrue()), bFalse());
//   rewrite.addRule(bNot(bNot(B)), B);

//   rewrite.addRule(bAnd(bFalse(), B), bFalse());
//   rewrite.addRule(bAnd(bTrue(), B), B);
//   rewrite.addRule(bAnd(B, bFalse()), bFalse());
//   rewrite.addRule(bAnd(B, bTrue()), B);

//   rewrite.addRule(bImplies(bFalse(), B), bTrue());
//   rewrite.addRule(bImplies(bTrue(), B), B);

//   rewrite.addRule(bOr(bFalse(), B), B);
//   rewrite.addRule(bOr(bTrue(), B), bTrue());
//   rewrite.addRule(bOr(B, bFalse()), B);
//   rewrite.addRule(bOr(B, bTrue()), bTrue());

//   rewrite.addRule(mEquals(N, N), bTrue());

//   rewrite.addRule(bEquals(bTrue(), B), B);
//   rewrite.addRule(bEquals(bFalse(), B), bNot(B));
//   rewrite.addRule(bEquals(B, bTrue()), B);
//   rewrite.addRule(bEquals(B, bFalse()), bNot(B));

//   rewrite.addRule(bAnd(B, B), B);
//   rewrite.addRule(bOr(B, B), B);

//   putRewriteSystem("simplifications", rewrite);
// }

string parseRewriteSystem(string &s, int &w, RewriteSystem &rs)
{
  skipWhiteSpace(s, w);
  matchString(s, w, "rewrite-system");
  skipWhiteSpace(s, w);
  string name = getIdentifier(s, w);
  skipWhiteSpace(s, w);
  while (w < len(s)) {
    skipWhiteSpace(s, w);
    ConstrainedTerm t = parseConstrainedTerm(s, w);
    skipWhiteSpace(s, w);
    matchString(s, w, "=>");
    skipWhiteSpace(s, w);
    FastTerm tp = parseTerm(s, w);
    rs.addRule(t, tp);
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
  Entry point to the RMT tool.
*/
int main(int argc, char **argv)
{
  initFastTerm();
  bool readFromStdin = false;
  char *filename = argv[argc - 1];
  for (int i = 1; i < argc; ++i) {
    if (i < argc - 1 && strcmp(argv[i], "-v") == 0) {
      char *end;
      debug_level = strtol(argv[i + 1], &end, 10);
      LOG(INFO, cerr << "Debuging level is " << debug_level);
      if (*end) {
        abortWithMessage("Syntax: ./rmt [-v <level>] file.in");
      }
    }
    if (strcmp(argv[i], "--in") == 0) {
      readFromStdin = true;
    }
  }

  ifstream inputf;
  if (!readFromStdin) {
    inputf.open(filename);
    if (!inputf.is_open()) {
      abortWithMessage(string("Cannot open file ") + filename + ".");
    }
  }
  istream &input = readFromStdin ? cin : inputf;
  string s;
  int w = 0;

  while (!input.eof()) {
    string tmp;
    getline(input, tmp);
    s += tmp;
    s += "\n";
  }
  if (!readFromStdin) {
    inputf.close();
  }

  // start_z3_api();

  // addPredefinedSorts();
  // addPredefinedFunctions();
  // createBuiltIns();
  // addPredefinedRewriteSystems();

  for (skipWhiteSpace(s, w); w < len(s); skipWhiteSpace(s, w)) {
    Query *query = NULL;
    if (lookAhead(s, w, "sorts")) {
      parseSorts(s, w);
    } else if (lookAhead(s, w, "subsort")) {
      parseSubsorts(s, w);
    } else if (lookAhead(s, w, "signature")) {
      parseFunctions(s, w);
    } else if (lookAhead(s, w, "variables")) {
      parseVariables(s, w);
    } else if (lookAhead(s, w, "rewrite-system")) {
      RewriteSystem rs;
      string name = parseRewriteSystem(s, w, rs);
      rewriteSystems[name] = rs;
      skipWhiteSpace(s, w);
    } else if (lookAhead(s, w, "abstract")) {
      processAbstract(s, w);
    }
    // else if (lookAhead(s, w, "builtins")) parseBuiltins(s, w);
    // else if (lookAhead(s, w, "assert")) parseAsserts(s, w);
    // else if (lookAhead(s, w, "define")) parseDefinedFunctions(s, w);
    else if ((query = Query::lookAheadQuery(s, w))) {
      query->parse(s, w);
      skipWhiteSpace(s, w);
      query->execute();
    } else if (lookAhead(s, w, "!EOF!")) {
      break;
    } else {
      expected("valid command", w, s);
    }
  }
  cout << "Done" << endl;
  // printDebugInformation();
  return 0;
}
