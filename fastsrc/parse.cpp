#include "parse.h"
#include "constrainedterm.h"
#include "log.h"
#include "helper.h"
#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;

struct ErrorLocation
{
  int line;
  int lineStart;
  int column;
};

void computeErrorLocation(ErrorLocation &errorLocation, int &where, string text)
{
  errorLocation.line = 1;
  errorLocation.column = 0;
  errorLocation.lineStart = -1;
  assert(where - 1 < len(text));
  for (int i = 0; i < where - 1; ++i) {
    if (text[i] == '\n') {
      errorLocation.line++;
      errorLocation.lineStart = i + 1;
      errorLocation.column = 0;
    }
    errorLocation.column++;
  }
}

void printErrorLocation(ErrorLocation &errorLocation, int &where, string text)
{
  for (int i = errorLocation.lineStart; text[i] != '\n' && i <= min(where + 16, len(text) - 1); ++i) {
    cout << text[i];
  }
  cout << endl;
  for (int i = errorLocation.lineStart; i < where; ++i) {
    cout << ' ';
  }
  cout << '^' << endl;
}

void parseError(string error, int &where, string text)
{
  ErrorLocation errorLocation;
  computeErrorLocation(errorLocation, where, text);
  cout << "Error: " << error << " at line " << errorLocation.line << ", column " << errorLocation.column << "." << endl;
  printErrorLocation(errorLocation, where, text);
  exit(0);
}

void expected(string what, int &where, string text)
{
  ErrorLocation errorLocation;
  computeErrorLocation(errorLocation, where, text);
  cout << "expected " << what << " at line " << errorLocation.line << ", column " << errorLocation.column << "." << endl;
  printErrorLocation(errorLocation, where, text);
  exit(0);
}

bool isIdentifierChar(char c)
{
  return c == '_' || c == '|' || isalpha(c) || isdigit(c); 
}

string getQuotedString(string &s, int &pos)
{
  string str = "";

  matchString(s, pos, "\"");
  while (pos < len(s) && s[pos] != '"') {
    str += s[pos];
    pos++;
  }
  matchString(s, pos, "\"");
  
  return str;
}

bool lookAheadIdentifier(string &s, int &pos)
{
  if (pos < len(s) && isIdentifierChar(s[pos])) {
    return true;
  }
  return false;
}

string getIdentifier(string &s, int &pos)
{
  string id = "";
  if (pos < len(s) && isIdentifierChar(s[pos])) {
    while (pos < len(s) && isIdentifierChar(s[pos])) {
      id += s[pos];
      pos++;
    }
  } else {
    expected("identifier", pos, s);
  }
  return id;
}

int getNumber(string &s, int &pos)
{
  int result;
  if (pos >= len(s) || !isdigit(s[pos])) {
    expected("number", pos, s);
  }
  string str;
  while (pos < len(s) && isdigit(s[pos])) {
    str += s[pos];
    pos++;
  }
  istringstream iss(str);
  iss >> result;
  return result;
}

void skipWhiteSpace(string &s, int &pos)
{
  bool progress;
  do {
    progress = false;
    while (pos < len(s) && (s[pos] == ' ' || s[pos] == '\t' || s[pos] == '\n' || s[pos] == '\r')) {
      pos++;
    }
    if (pos + 1 < len(s) && s[pos] == '/' && s[pos + 1] == '*') {
      match(s, pos, '/');
      match(s, pos, '*');
      while (pos + 1 < len(s) && !(s[pos] == '*' && s[pos + 1] == '/')) {
        pos++;
      }
      if (pos + 1 >= len(s)) {
        expected("end of comment", pos, s);
      }
      match(s, pos, '*');
      match(s, pos, '/');
      progress = true;
    }
    if (pos + 1 < len(s) && s[pos] == '/' && s[pos + 1] == '/') {
      match(s, pos, '/');
      match(s, pos, '/');
      while (pos < len(s) && s[pos] != '\n' && s[pos] != '\r') {
        pos++;
      }
      if (pos < len(s)) {
        if (s[pos] == '\n') {
          match(s, pos, '\n');
        }
        if (pos < len(s) && s[pos] == '\r') {
          match(s, pos, '\r');
        }
      }
      progress = true;
    }
  } while (progress);
}

bool lookAhead(string &s, int &pos, string what)
{
  for (int i = 0; i < len(what); ++i) {
    if (pos + i >= len(s) || s[pos + i] != what[i]) {
      return false;
    }
  }
  //if I am parsing alphanumeric string, I want to have non-alphanumeric character after
  if (isalnum(what[0])) {
    int lastpos = pos + len(what);
    if (lastpos < len(s) && isalnum(s[lastpos])) {
      return false;
    }
  }
  return true;
}

void matchString(string &s, int &pos, string what)
{
  for (int i = 0; i < len(what); ++i) {
    if (!(pos + i < len(s) && s[pos + i] == what[i])) {
      ostringstream oss;
      oss << "\"" << what << "\"";
      expected(oss.str(), pos, s);
    }
  }
  pos += len(what);
}

void match(string &s, int &pos, char c)
{
  if (pos < len(s) && s[pos] == c) {
    pos++;
  } else {
    ostringstream oss;
    oss << c;
    expected(oss.str(), pos, s);
  }
}

FastTerm parseTerm(string &s, int &pos)
{
  skipWhiteSpace(s, pos);
  string id;
  if (lookAhead(s, pos, "(")) {
    match(s, pos, '(');
    skipWhiteSpace(s, pos);
    id = getIdentifier(s, pos);
    if (!existsFunc(id.c_str())) {
      parseError("unknown function symbol " + id, pos, s);
    }
    FastFunc f = getFuncByName(id.c_str());
    if (getArity(f) > 0) {
      vector<FastTerm> arguments;
      for (uint32 i = 0; i < getArity(f); ++i) {
        FastTerm t = parseTerm(s, pos);
	FastSort sorti = getSort(t);
	if (!isSubSortTransitive(sorti, getArgSort(f, i))) {
	  cerr << "Term " << toString(t) << " has sort " << getSortName(sorti) << endl;

	  cerr << "Cannot use " << toString(t) << " as argument #" << (i + 1) << " of " << id << " (expecting term of sort " << getSortName(getArgSort(f, i)) << ")" << endl;
	  
	  parseError("incorrect subsorting", pos, s);
	}
        skipWhiteSpace(s, pos);
        arguments.push_back(t);
      }
      match(s, pos, ')');
      FastTerm result = newFuncTerm(f, &arguments[0]);
      return result;
    } else {
      assert(getArity(f) == 0);
      return newFuncTerm(f, 0);
    }
  } else {
    id = getIdentifier(s, pos);
    skipWhiteSpace(s, pos);
    if (existsFunc(id.c_str())) {
      FastFunc f = getFuncByName(id.c_str());
      if (getArity(f) != 0) {
	parseError("function " + id + "should have arguments", pos, s);
      }
      return newFuncTerm(f, 0);
    } else {
      if (!existsVar(id.c_str())) {
	parseError(("unknown identifier " + id).c_str(), pos, s);
      }
      FastVar v = getVarByName(id.c_str());
      return v;
    }
  }
  LOG(DEBUG9, cerr << "impossible !!!");
  assert(0);
  return 0;
}

// Term *parseTerm(string &s)
// {
//   int position = 0;
//   return parseTerm(s, position);
// }

// Term *parseTerm(const char *s)
// {
//   string str(s);
//   return parseTerm(str);
// }

ConstrainedTerm parseConstrainedTerm(string &s, int &w)
{
  skipWhiteSpace(s, w);
  FastTerm term = parseTerm(s, w);
  FastTerm constraint;
  skipWhiteSpace(s, w);
  if (lookAhead(s, w, "/\\")) {
    matchString(s, w, "/\\");
    skipWhiteSpace(s, w);
    constraint = parseTerm(s, w);
  } else {
    constraint = fastTrue();
  }
  return ConstrainedTerm(term, constraint);
}

// ConstrainedPair parseConstrainedPair(string &s, int &w) {
//   LOG(DEBUG) << "Parsing constrained term pair" << endl;
//   skipWhiteSpace(s, w);
//   Term *lhs = parseTerm(s, w);
//   LOG(DEBUG) << "Parsed lhs term: " << lhs->toString() << endl;
//   Term *rhs = parseTerm(s, w);
//   LOG(DEBUG) << "Parsed rhs term: " << rhs->toString() << endl;
//   Term *constraint;
//   skipWhiteSpace(s, w);
//   if (lookAhead(s, w, "/\\")) {
//     matchString(s, w, "/\\");
//     skipWhiteSpace(s, w);
//     constraint = parseTerm(s, w);
//     LOG(DEBUG) << "Parsed constraint term: " << constraint->toString() << endl;
//   }
//   else {
//     constraint = bTrue();
//     assert(constraint);
//     LOG(DEBUG) << "Implicit constraint: " << constraint->toString() << endl;
//   }
//   return ConstrainedPair(lhs, rhs, constraint);
// }

// ConstrainedRewriteSystem parseCRSfromName(string &s, int &w) {
//   skipWhiteSpace(s, w);
//   string crsName = getIdentifier(s, w);
//   skipWhiteSpace(s, w);
//   if (!existsConstrainedRewriteSystem(crsName)) {
//     LOG(ERROR) << "No CRS exists with name " << crsName << endl;
//     expected("Existing CRS", w, s);
//   }
//   return getConstrainedRewriteSystem(crsName);
// }
