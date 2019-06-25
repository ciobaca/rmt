#include "smallqueries.h"
#include "fastterm.h"
#include "parse.h"
#include "abstract.h"
#include "helper.h"
#include <iostream>

using namespace std;

void processAbstract(string &s, int &w)
{
  skipWhiteSpace(s, w);
  matchString(s, w, "abstract");
  skipWhiteSpace(s, w);
  FastTerm term = parseTerm(s, w);
  skipWhiteSpace(s, w);
  match(s, w, ';');
  skipWhiteSpace(s, w);

  FastTerm resultTerm;
  FastSubst resultSubst;
  abstractTerm(term, resultTerm, resultSubst);
  cout << "Abstracting          " << toStringFT(term) << ":" << endl;
  cout << "Abstract term     =  " << toStringFT(resultTerm) << endl;
  //  cout << "Abstracting subst =  " << toStringSubst(resultTerm) << endl;
}
