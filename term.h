#ifndef TERM_H__
#define TERM_H__

#include <string>
#include <vector>

#include "name.h"
#include "variable.h"
#include "function.h"
#include "rewrite.h"
#include "solution.h"
#include "constrainedsolution.h"
#include "substitution.h"
#include "helper.h"

using namespace std;

struct Substitution;
struct FunTerm;
struct VarTerm;
//struct NamTerm;
struct KnowledgeBase;
struct ConstrainedTerm;
struct CRewriteSystem;

struct Term
{
  virtual string toString() = 0;

  virtual Sort *getSort() = 0;

  bool computedVars;
  vector<Variable *> allVars;

  Term() {
    computedVars = false;
  }

  // Returns the set of variables that appear in the term.  The result
  // is cached (a second call to vars will be O(1)).
  virtual vector<Variable *> vars();

  // Compute the set of names that appear in the term.  virtual
  //  vector<Name *> names() = 0;

  // Apply the substitution given as a parameter to this term.
  virtual Term *substitute(Substitution &);

  // Helper function that applies the substitution given as the first
  // parameter.  The second parameter is a cache that records for
  // every (sub)term the result.  The cache allows to save computation
  // time if there are several equal subterms.  For example, when
  // appling the substitution sigma to f(g(x, y), g(x, y)), the cache
  // will contain at the end x |-> sigma(x); y |-> sigma(y); g(x, y)
  // |-> \sigma(g(x, y)) and f(g(x, y), g(x, y)) |-> sigma(f(g(x, y),
  // g(x, y))). In this way, the computation (applying the
  // substitution) is actually performed once for each subterm that is
  // different from previous subterms. If a subterm has been seen
  // before, the result is obtained from the cache.
  virtual Term *computeSubstitution(Substitution &, map<Term *, Term *> &) = 0;

  // Compute the set of variable appearing in the term. O(|dag|).
  virtual vector<Variable *> computeVars() = 0;

  // Check if the term is in normal form w.r.t. the given rewrite
  // system.
  virtual bool isNormalized(RewriteSystem &rewriteSystem);

  // Compute a normal form of the term w.r.t. the given rewrite
  // system.  Can loop forever if the rewrite system is not
  // terminating.  The normalization is quite fast, using a cache that
  // remembers for each subterm appearing in the computation the
  // normalized form.
  virtual Term *normalize(RewriteSystem &rewriteSystem);

  // Unifies this term with the parameter. Implements visitor pattern
  // for multiple dispatch.  The substitution is the substitution
  // compute so far.
  virtual bool unifyWith(Term *, Substitution &) = 0;

  // Unifies this term with the parameter. Implements visitor pattern
  // for multiple dispatch.  The substitution is the substitution
  // compute so far.
  virtual bool unifyWithFunTerm(FunTerm *, Substitution &) = 0;

  // Unifies this term with the parameter. Implements visitor pattern
  // for multiple dispatch.  The substitution is the substitution
  // compute so far.
  virtual bool unifyWithVarTerm(VarTerm *, Substitution &) = 0;

  // Unifies this term with the parameter. Implements visitor pattern
  // for multiple dispatch.  The substitution is the substitution
  // compute so far.  virtual bool unifyWithNamTerm(NamTerm *,
  // Substitution &) = 0;

  // Deprecated. Used in the "kiss" tool to generate the initial
  // knowledge base.
  virtual vector<pair<Term *, Term *> > split() = 0;

  // Checks whethere the current term is an instance of the parameter
  // (i.e. if there exists a substitution sigma such that *this =
  // \sigma(parameter)). If the result is true, then the second
  // parameter will contain the witness substitution.
  virtual bool isInstanceOf(Term *, Substitution &);

  // Checks if the term is a variable.
  virtual bool isVariable() = 0;

  virtual ~Term() {}

  // Computes if a term is in normal form w.r.t. the given rewrite
  // system.  The computation is quite fast, using a cache (the second
  // parameter) that remembers for each subterm if it is in normal
  // form or not.  You should probably use "isNormalized()" instead of
  // this function.
  virtual bool computeIsNormalized(RewriteSystem &, map<Term *, bool> &) = 0;

  // Compute a normal form of the term w.r.t. the given rewrite
  // system.  Can loop forever if the rewrite system is not
  // terminating.  The normalization is quite fast, using a cache (the
  // second parameter) that remembers for each subterm appearing in
  // the computation the normalized form. You should probably use
  // "normalize()" instead of this function.
  virtual Term *computeNormalize(RewriteSystem &, map<Term *, Term *> &) = 0;

  // Checks whethere the current term is an instance of the parameter
  // (i.e. if there exists a substitution sigma such that *this =
  // \sigma(parameter)). If the result is true, then the second
  // parameter will contain the witness substitution. Implements visitor pattern
  // for multiple dispatch.
  virtual bool computeIsInstanceOf(Term *, Substitution &, map<pair<Term *, Term *>, bool> &) = 0;

  // Checks whethere the current term is a generalization of the
  // parameter, or equivalently, if the parameter is an instance of
  // the current term.  (i.e. if there exists a substitution sigma
  // such that \sigma(*this) = parameter). If the result is true, then
  // the second parameter will contain the witness substitution. The
  // third parameter is a cache that avoids repeating the same
  // computation. Implements visitor pattern for multiple dispatch,
  // together with computeIsInstanceOf.  virtual bool
  // computeIsGeneralizationOf(NamTerm *, Substitution &,
  // map<pair<Term *, Term *>, bool> &) = 0;
  virtual bool computeIsGeneralizationOf(VarTerm *, Substitution &, map<pair<Term *, Term *>, bool> &) = 0;

  // Checks whethere the current term is a generalization of the
  // parameter, or equivalently, if the parameter is an instance of
  // the current term.  (i.e. if there exists a substitution sigma
  // such that \sigma(*this) = parameter). If the result is true, then
  // the second parameter will contain the witness substitution. The
  // third parameter is a cache that avoids repeating the same
  // computation. Implements visitor pattern for multiple dispatch,
  // together with computeIsInstanceOf.  virtual bool
  // computeIsGeneralizationOf(NamTerm *, Substitution &,
  // map<pair<Term *, Term *>, bool> &) = 0;
  virtual bool computeIsGeneralizationOf(FunTerm *, Substitution &, map<pair<Term *, Term *>, bool> &) = 0;

  // Performs one top-most rewrite w.r.t. the given rewrite system.
  // Returns this if no rewrite is possible.
  // Fill in substitution with the witness to the rewrite.
  virtual Term *rewriteTopMost(RewriteSystem &, Substitution &how);

  // Performs one top-most rewrite w.r.t. the given rewrite rule.
  // Returns this if not possible.
  // Fill in substitution with the witness to the rewrite.
  virtual Term *rewriteTopMost(pair<Term *, Term *>, Substitution &how);

  // Performs one rewrite step w.r.t. the given rewrite system (not
  // necessarily top-most).  Returns this if no rewrite is possible.
  // Fill in substitution with the witness to the rewrite.
  virtual Term *rewriteOneStep(RewriteSystem &, Substitution &how) = 0;

  // Performs one rewrite step w.r.t. the given rewrite system (not
  // necessarily top-most).  Returns this if no rewrite is possible.
  // Fill in substitution with the witness to the rewrite.
  virtual Term *rewriteOneStep(pair<Term *, Term *>, Substitution &how) = 0;


  // Performs a one-step rewrite search, i.e. finds all terms to which
  // this reduces in one step.
  virtual vector<Solution> rewriteSearch(RewriteSystem &) = 0;

  // Performs a one-step narrowing search, i.e. finds all terms to which
  // this term narrows in one step.
  virtual vector<Solution> narrowSearch(RewriteSystem &) = 0;

  // Performs a one-step narrowing search, i.e. finds all terms to which
  // this term narrows in one step.
  virtual vector<ConstrainedSolution> narrowSearch(CRewriteSystem &) = 0;

  // Performs a one-step narrowing search, offloading interpreted terms
  // to the SMT solver.
  virtual vector<ConstrainedTerm> smtNarrowSearch(RewriteSystem &, Term *initialConstraint);

  // Performs a one-step narrowing search, offloading interpreted terms
  // to the SMT solver.
  virtual vector<ConstrainedTerm> smtNarrowSearch(CRewriteSystem &, Term *initialConstraint);

  bool smtUnifyWith(Term *other, Term *initialConstraint,
		    Substitution &resultSubstitution, Term *&resultConstraint);

  // Returns the number of different subterms (the same subterm is
  // counted only once, even if it appears in different places).  Uses
  // computeDagSize, which caches the results, ensuring O(|dagSize()|)
  // running time.
  virtual int dagSize();

  // Computed the number of different subterms (the same subterm is
  // counted only once, even if it appears in different places). The
  // parameter is a cache that eliminated duplicate computations.
  virtual int computeDagSize(map<Term *, int> &) = 0;

  // Returns the abstraction of a term w.r.t. the subterms that are
  // interpreted. The abstraction is a generalization of the term
  // where all subterms that are interpreted are replaced with
  // variables. Two different occurences of the same subterm must be
  // replaced with different variables.  The function returns the
  // abstracted term and the first parameter contains the witnessing
  // substitution.
  virtual Term *abstract(Substitution &) = 0;

  // return the reprezentation of a term in smt-speak (Z3)
  virtual string toSmtString() = 0;
};

void logmgu(string, Term *, Term *, Substitution &);

#endif
