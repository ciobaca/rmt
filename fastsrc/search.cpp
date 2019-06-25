#include "search.h"
#include "constrainedterm.h"
#include "log.h"

std::vector<ConstrainedTerm> search(const ConstrainedTerm &ct, const RewriteSystem &rs,
		       uint32 minDepth, uint32 maxDepth)
{
  LOG(ERROR, std::cerr << "search :: not yet implemented");
  exit(-1);
  return { ConstrainedTerm(0, 0) };
}

std::vector<ConstrainedTerm> searchOneStep(const ConstrainedTerm &ct, const RewriteSystem &rs)
{
  FastTerm term = ct.term;
  FastTerm phi = ct.constraint;
  
  for (uint i = 0; i < rs.size(); ++i) {
    FastTerm lhs = rs[i].first.term;
    FastTerm rhs = rs[i].second;
    FastTerm phiRule = rs[i].first.constraint;

    
  }
  return { ConstrainedTerm(0, 0) };
}
