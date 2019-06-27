#ifndef SMALLQUERIES_H__
#define SMALLQUERIES_H__

#include <string>

void processAbstract(std::string &s, int &w);
void processSearch(std::string &s, int &w);
void processUnify(std::string &s, int &w);
void processSmtUnify(std::string &s, int &w);
void processSatisfiability(std::string &s, int &w);
void processCompute(std::string &s, int &w);
void processDefinedSearch(std::string &s, int &w);

#endif
