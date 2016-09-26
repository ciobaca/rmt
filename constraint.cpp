// #include "constraint.h"
// #include "term.h"
// #include <sstream>

// using namespace std;

// string Constraint::toString()
// {
//   ostringstream oss;
  
//   oss << "(";
//   for (int i = 0; i < equalities.size(); ++i) {
//     oss << "(";
//     oss << "= ";
//     oss << equalities[i].first->toString();
//     oss << " ";
//     oss << equalities[i].second->toString();
//     oss << ")";
//     if (i != equalities.size() - 1) {
//       oss << " /\\ ";
//     }
//   }
//   oss << ")";

//   return oss.str();
// }
