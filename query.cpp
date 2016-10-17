#include "query.h"
#include "parse.h"
#include "querysmtimplies.h"
#include "querysmtunify.h"
#include "querysmtrun.h"
#include "querysmtnarrowsearch.h"
#include "querysmtprove.h"
#include "querysmtsatisfiability.h"

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

Query *createQuerySmtNarrowSearch()
{
  return new QuerySmtNarrowSearch();
}

Query *createQuerySmtUnify()
{
  return new QuerySmtUnify();
}

Query *createQuerySmtImplies()
{
  return new QuerySmtImplies();
}

Query *createQuerySmtProve()
{
  return new QuerySmtProve();
}

Query *createQuerySmtSatisfiability()
{
  return new QuerySmtSatisfiability();
}

Query *createQuerySmtRun()
{
  return new QuerySmtRun();
}

std::map<std::string, QueryCreator> *registerQueries()
{
  std::map<std::string, QueryCreator> *map;
  map = new std::map<std::string, QueryCreator>;
  (*map)["search"] = createQuerySmtNarrowSearch;
  (*map)["unify"] = createQuerySmtUnify;
  (*map)["implies"] = createQuerySmtImplies;
  (*map)["prove"] = createQuerySmtProve;
  (*map)["run"] = createQuerySmtRun;
  (*map)["satisfiability"] = createQuerySmtSatisfiability;
  return map;
}

std::map<std::string, QueryCreator> *Query::commands = registerQueries();  
