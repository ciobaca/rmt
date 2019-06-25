#ifndef SEARCH_H__
#define SEARCH_H__

#include "search.h"
#include "constrainedterm.h"
#include "rewritesystem.h"
#include "constrainedterm.h"

std::vector<ConstrainedTerm> search(const ConstrainedTerm &ct, const RewriteSystem &rs,
		       uint32 minDepth, uint32 maxDepth);

std::vector<ConstrainedTerm> searchOneStep(const ConstrainedTerm &ct, const RewriteSystem &rs);

#endif
