#include "query.h"
#include "parse.h"
#include "queryimplies.h"
#include "queryinstrument.h"
#include "queryinstrument_c.h"
#include "queryunify.h"
#include "queryrun.h"
#include "querysearch.h"
#include "queryder.h"
#include "querysubs.h"
#include "querycirc.h"
#include "queryprovereachability.h"
#include "queryproveequivalence.h"
#include "querysatisfiability.h"

#include <string>
#include <map>

using namespace std;

void Query::registerQueryCreator(std::string command,
					   QueryCreator constructor)
{
  (*commands)[command] = constructor;
}
 
Query *Query::lookAheadQuery(std::string &s, int &w)
{
  typedef std::map<std::string, QueryCreator>::iterator it_type;
    
  for(it_type it = (*commands).begin(); it != (*commands).end(); it++) {
    string command = it->first;
    if (lookAhead(s, w, command)) {
      QueryCreator nqc = it->second;
      return nqc();
    }
  }
  return 0;
}

Query *createQueryDer()
{
  return new QueryDer();
}

Query *createQuerySubs()
{
  return new QuerySubs();
}

Query *createQueryCirc()
{
  return new QueryCirc();
}

Query *createQuerySearch()
{
  return new QuerySearch();
}

Query *createQueryUnify()
{
  return new QueryUnify();
}

Query *createQueryImplies()
{
  return new QueryImplies();
}

Query *createQueryInstrument()
{
  return new QueryInstrument();
}

Query *createQueryInstrument_C()
{
  return new QueryInstrument_C();
}

Query *createQueryProveReachability()
{
  return new QueryProveReachability();
}

Query *createQueryProveEquivalence()
{
  return new QueryProveEquivalence();
}

Query *createQuerySatisfiability()
{
  return new QuerySatisfiability();
}

Query *createQueryRun()
{
  return new QueryRun();
}

std::map<std::string, QueryCreator> *registerQueries()
{
  std::map<std::string, QueryCreator> *map;
  map = new std::map<std::string, QueryCreator>;
  (*map)["search"] = createQuerySearch;
  (*map)["der"] = createQueryDer;
  (*map)["subs"] = createQuerySubs;
  (*map)["circ"] = createQueryCirc;
  (*map)["unify"] = createQueryUnify;
  (*map)["implies"] = createQueryImplies;
  (*map)["instrument"] = createQueryInstrument;
  (*map)["cinstrument"] = createQueryInstrument_C;
  (*map)["prove"] = createQueryProveReachability;
  (*map)["show-equivalent"] = createQueryProveEquivalence;
  (*map)["run"] = createQueryRun;
  (*map)["satisfiability"] = createQuerySatisfiability;
  return map;
}

std::map<std::string, QueryCreator> *Query::commands = registerQueries();  
