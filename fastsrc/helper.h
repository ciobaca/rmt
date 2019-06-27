#ifndef HELPER_H__
#define HELPER_H__

#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <functional>
#include <ctime>
#include "fastterm.h"

/* #ifndef NDEBUG */
/* #define NDEBUG */
/* #endif */

#define len(x) ((int)x.size())

//#define LOG_SOLVING 1
//#define LOG_COMBINE 3

/* #define SMALL_SOLVED "\\x" */
/* #define BIG_SOLVED "\\X" */

/* #define SMALL_UNSOLVED "\\y" */
/* #define BIG_UNSOLVED "\\Y" */

/* #define SMALL_TEMP "\\z" */
/* #define BIG_TEMP "\\Z" */

/* #define REWRITE "\\t" */

/* struct Variable; */
/* struct Substitution; */

/* using namespace std; */

template<typename A, typename B> bool contains(const std::map<A, B> &container, const A &elem)
{
  return container.find(elem) != container.end();
}

template<typename A, typename B> bool contains(const std::unordered_map<A, B> &container, const A &elem)
{
  return container.count(elem);
}

template<typename A, typename B> bool contains(const std::vector<std::pair<A, B>> &container, const A &elem) {
  return find_if(container.begin(), container.end(), [&](const std::pair<A, B> &it) { return it.first == elem; }) != container.end();
}

template<typename T> bool contains(const std::vector<T> &container, const T &elem)
{
  return find(container.begin(), container.end(), elem) != container.end();
}

/* template<typename T> bool subseteq(vector<T> a, vector<T> b) */
/* { */
/*   for (int i = 0; i < len(a); ++i) { */
/*     if (!contains(b, a[i])) { */
/*       return false; */
/*     } */
/*   } */
/*   return true; */
/* } */

template<typename T> void append(std::vector<T> &a, const std::vector<T> &b)
{
  for (int i = 0; i < len(b); ++i) {
    a.push_back(b[i]);
  }
}

/* template<typename T> vector<T> unique(vector<T> a) */
/* { */
/*   sort(a.begin(), a.end()); */
/*   a.erase(unique(a.begin(), a.end()), a.end()); */
/*   return a; */
/* } */

/* map<Variable *, Variable *> createRenaming(vector<Variable *> v, string s); */

/* Substitution createSubstitution(map<Variable *, Variable *> r); */

/* string varVecToString(vector<Variable *> vars); */

std::string spaces(int);
std::string string_from_int(int);

extern int VERBOSITY;

void varsOf(FastTerm term, std::vector<FastVar> &vars);
std::vector<FastVar> uniqueVars(FastTerm term);
void abortWithMessage(const std::string &error);
std::string toString(FastTerm term);
std::string toString(FastSubst subst);

/* void addDefinedSuccessors(std::vector< std::pair<ConstrainedSolution, bool> > &successors, */
/*   Term *term, Term *constraint, bool progress, int depth); */

/* void printDebugInformation(); */

/* void updateTimer(string timer, clock_t val); */

/* void updateTimerIfFlag(string name, clock_t val); */

/* bool isTimerFlagSet(string timer); */

/* void setTimerFlag(string name); */

/* void unsetTimerFlag(string name); */

#endif
