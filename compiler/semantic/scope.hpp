#ifndef LLANG_SEMANTIC_SCOPE_HPP_INCLUDED
#define LLANG_SEMANTIC_SCOPE_HPP_INCLUDED

#include <boost/shared_ptr.hpp>
#include <map>

#include "common/identifier.hpp"

namespace llang {
namespace semantic {

class Symbol;

class Scope {
public:
	Scope(Scope* parent = 0) : parent_(parent) {}

	void addSymbol(Symbol* symbol);
	Symbol* lookup(const identifier_t& name);

	Scope* parent() { return parent_; }
	const Scope* parent() const { return parent_; }

private:
	typedef std::map<identifier_t, Symbol*> symbol_map_t;

	Scope* parent_;

	symbol_map_t symbols;
};

} // namespace semantic
} // namespace llang

#endif
