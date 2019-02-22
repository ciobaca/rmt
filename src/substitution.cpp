#include "substitution.h"
#include "helper.h"
#include "term.h"
#include "varterm.h"
#include <cassert>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;

Substitution::Substitution() {
}

Substitution::Substitution(Variable *v, Term *t) {
  if (!(t->isVarTerm && v == t->getAsVarTerm()->variable)) {
    this->emplace_back(v, t);
  }
}

void Substitution::add(Variable *v, Term *t) {
  if (!inDomain(v) || (t->isVarTerm && v == t->getAsVarTerm()->variable)) {
    this->emplace_back(v, t);
  }
}

void Substitution::apply(Substitution &s) {
  for (auto &it : *this) {
    it.second = it.second->substitute(s);
  }
}

void Substitution::force(Variable *v, Term *t) {
  Substitution temp;
  temp.add(v, t);
  apply(temp);
  add(v, t);
}

void Substitution::compose(Substitution s) {
  s.apply(*this);
  for (int i = 0; i < (int)this->size(); ++i) {
    if (s.inDomain((*this)[i].first)) {
      std::swap((*this)[i], (*this)[this->size() - 1]);
      this->pop_back();
      --i;
    }
  }
  for (auto &it : s) {
    this->add(it.first, it.second);
  }
}

void Substitution::compose(pair<Variable*, Term*> s) {
  s.second = s.second->substitute(*this);
  for (int i = 0; i < (int)this->size(); ++i) {
    if (s.first == (*this)[i].first) {
      std::swap((*this)[i], (*this)[this->size() - 1]);
      this->pop_back();
      --i;
    }
  }
  this->add(s.first, s.second);
}

bool Substitution::inDomain(Variable *v) {
  return find_if(this->begin(), this->end(), [&](const pair<Variable*, Term*> &it) { return it.first == v; }) != this->end();
}

bool Substitution::inRange(Variable *v) {
  return find_if(this->begin(), this->end(), [&](const pair<Variable*, Term*> &it) { return it.second->hasVariable(v); }) != this->end();
}

Term *Substitution::image(Variable *v) {
  auto ans = find_if(this->begin(), this->end(), [&](const pair<Variable*, Term*> &it) { return it.first == v; });
  if (ans != this->end()) {
    return ans->second;
  }
  throw std::runtime_error("Substitution does not contain needed variable");
}

string Substitution::toString() {
  if (!this->size()) {
    return "( )";
  }
  ostringstream oss;
  oss << "( ";
  auto lastItem = *(this->rbegin());
  for (const auto &it : *this) {
    oss << it.first->name << " |-> " << it.second->toString() << (it == lastItem  ? " )" : "; ");
  }
  return oss.str();
}
