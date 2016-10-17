#include "query.h"
#include "parse.h"
#include "queryimplies.h"
#include "queryunify.h"
#include "queryrun.h"
#include "querysearch.h"
#include "queryprove.h"
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

Query *createQueryProve()
{
  return new QueryProve();
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
  (*map)["unify"] = createQueryUnify;
  (*map)["implies"] = createQueryImplies;
  (*map)["prove"] = createQueryProve;
  (*map)["run"] = createQueryRun;
  (*map)["satisfiability"] = createQuerySatisfiability;
  return map;
}

std::map<std::string, QueryCreator> *Query::commands = registerQueries();  
