#ifndef PARSE_H__
#define PARSE_H__

#include <string>
#include "fastterm.h"
#include "constrainedterm.h"

using namespace std;

ConstrainedTerm parseConstrainedTerm(string &s, int &w);
// FastTerm parseTerm(string &s);
/* FastTerm parseTerm(const char *s); */
void matchString(std::string &s, int &pos, std::string what);
void match(std::string &s, int &pos, char c);
void skipWhiteSpace(std::string &s, int &pos);
std::string getQuotedString(std::string &s, int &pos);
bool lookAhead(std::string &s, int &pos, std::string what);
bool lookAheadIdentifier(std::string &s, int &pos);
std::string getIdentifier(std::string &s, int &pos);
int getNumber(std::string &s, int &pos);

FastTerm parseTerm(std::string &s, int &pos);

void expected(std::string what, int &where, std::string text);
void parseError(std::string error, int &where, std::string text);
/* ConstrainedRewriteSystem parseCRSfromName(string &s, int &w); */
/* ConstrainedPair parseConstrainedPair(string &s, int &w); */

#endif
