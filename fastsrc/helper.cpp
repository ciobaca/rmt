#include "helper.h"
#include <algorithm>
#include "fastterm.h"
#include "log.h"
#include <sstream>
#include <iostream>
#include <ctime>

using namespace std;

void abortWithMessage(const std::string &error)
{
  cout << "Error: " << error << endl;
  exit(0);
}

extern uint32 termData[MAXDATA];
extern uint32 termDataSize;

void printToOss(FastTerm term, ostringstream &oss)
{
  if (isVariable(term)) {
    assert(/* 0 <= term && */term < MAXVARS);
    oss << getVarName(term);
  } else {
    assert(isFuncTerm(term));
    assert(term >= MAXVARS);
    size_t index = term - MAXVARS;
    assert(/* 0 <= index && */ index < termDataSize);

    FastFunc func = termData[index]; 
    if (getArity(func) > 0) {
      oss << "(";
    }
    oss << getFuncName(func);
    for (uint i = 0; i < getArity(func); ++i) {
      oss << " ";
      printToOss(termData[index + i + 1], oss);
    }
    if (getArity(func) > 0) {
      oss << ")";
    }
  }
}

std::string toString(FastTerm term)
{
  ostringstream oss;
  printToOss(term, oss);
  return oss.str();
}

std::string toString(FastSubst subst)
{
  ostringstream oss;
  oss << "{ ";
  for (uint i = 0; i < subst.count; i += 2) {
    if (i > 0) {
      oss << ", ";
    }
    assert(validFastTerm(subst.data[i + 1]));
    assert(isVariable(subst.data[i]));
    printToOss(subst.data[i], oss);
    oss << " |-> ";
    assert(validFastTerm(subst.data[i + 1]));
    printToOss(subst.data[i + 1], oss);
  }
  oss << " }";
  return oss.str();
}

// map<Variable *, Variable *> createRenaming(vector<Variable *>v, string s)
// {
//   map<Variable *, Variable *> result;

//   int counter = 0;
//   for (int i = 0; i < len(v); ++i) {
//     if (contains(result, v[i])) {
//       continue;
//     }
//     ostringstream oss;
//     oss << s << counter;
//     result[v[i]] = getInternalVariable(oss.str(), v[i]->sort);
//     counter++;
//   }

//   return result;
// }

// Substitution createSubstitution(map<Variable *, Variable *> r)
// {
//   Substitution subst;
//   for (map<Variable *, Variable *>::iterator it = r.begin();
//     it != r.end(); ++it) {
//     subst.add(it->first, getVarTerm(it->second));
//   }
//   return subst;
// }

// string varVecToString(vector<Variable *> vars)
// {
//   ostringstream oss;
//   for (int i = 0; i < (int)vars.size(); ++i) {
//     oss << vars[i]->name << " ";
//   }
//   return oss.str();
// }

string spaces(int tabs)
{
  ostringstream oss;
  for (int i = 0; i < tabs; ++i) {
    oss << "    ";
  }
  return oss.str();
}

string string_from_int(int number)
{
  ostringstream oss;
  oss << number;
  return oss.str();
}

// map< string, pair<clock_t, int> > _timers;
// set< string > _timerFlags;

// void updateTimer(string timer, clock_t val) {
//   _timers[timer].first += val;
//   _timers[timer].second += 1;
// }

// bool isTimerFlagSet(string timer) {
//   return _timerFlags.count(timer);
// }

// void updateTimerIfFlag(string name, clock_t val) {
//   if (!isTimerFlagSet(name)) return;
//   updateTimer(name, val);
// }

// void setTimerFlag(string name) {
//   _timerFlags.insert(name);
// }

// void unsetTimerFlag(string name) {
//   _timerFlags.erase(name);
// }

// void addDefinedSuccessors(vector< pair<ConstrainedSolution, bool> > &successors,
//   Term *term, Term *constraint, bool progress, int depth) {
//   ConstrainedTerm ct = ConstrainedTerm(term, constraint);
//   ConstrainedSolution *sol = NULL;
//   bool step = false;
//   do {
//     vector<Function*> funcs = ct.getDefinedFunctions();
//     step = false;
//     for (int fidx = 0; fidx < (int)funcs.size() && !step; ++fidx) {
//       vector<Function*> currentFunction;
//       currentFunction.push_back(funcs[fidx]);
//       vector<ConstrainedSolution> sols = ct.smtRewriteDefined(depth, getDefinedFunctionsSystem(currentFunction));
//       vector<ConstrainedTerm> ctsuccs = solutionsToSuccessors(sols);
//       if ((ctsuccs.size() > 0) &&
//         (//(sol == NULL) /*force at least one defined step*/ ||
//         ((int)ctsuccs.size() == ct.term->nrFuncInTerm(funcs[fidx])))) {
//         ct = ctsuccs[0];
//         sol = new ConstrainedSolution(sols[0]);
//         step = true;
//       }
//     }
//   } while (step);
//   if (sol != NULL) {
//     successors.push_back(make_pair(*sol, progress));
//   }
// }

// void printDebugInformation() {
//   for (const auto &it : _timers) {
//     cout << it.first << " was called " << it.second.second << " times " << endl;
//     double secs = 1.0 * it.second.first / CLOCKS_PER_SEC;
//     cout << it.first << " took in total " << secs << " seconds " << endl;
//     cout << it.first << "s took on average " << secs / it.second.second << " seconds " << endl;
//   }
// }

void varsOf(FastTerm term, vector<FastVar> &vars)
{
  if (isVariable(term)) {
    vars.push_back(term);
  } else {
    assert(isFuncTerm(term));
    FastFunc func = getFunc(term);
    for (uint i = 0; i < getArity(func); ++i) {
      FastTerm nt = getArg(term, i);
      varsOf(nt, vars);
    }
  }
}

vector<FastVar> uniqueVars(FastTerm term)
{
  vector<FastVar> result;
  varsOf(term, result);
  std::sort(result.begin(), result.begin());
  auto it = std::unique(result.begin(), result.end());
  result.resize(std::distance(result.begin(), it));
  return result;
}
