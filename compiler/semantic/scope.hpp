#ifndef LLANG_SEMANTIC_SCOPE_HPP_INCLUDED
#define LLANG_SEMANTIC_SCOPE_HPP_INCLUDED

#include <map>
#include <utility>

#include "common/identifier.hpp"
#include "semantic/symbol_ptr.hpp"

namespace llang {
namespace semantic {

class Scope {
public:
	Scope(Scope* parent = 0) : parent_(parent) {}

	void addSymbol(SymbolPtr symbol);
	SymbolPtr lookup(const identifier_t& name);

	Scope* parent() { return parent_; }
	const Scope* parent() const { return parent_; }

private:
	typedef std::map<identifier_t, SymbolPtr> symbol_map_t;

	Scope* parent_;

	symbol_map_t symbols;
};

} // namespace semantic
} // namespace llang

#endif
