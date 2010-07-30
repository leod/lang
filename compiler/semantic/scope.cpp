#include "semantic/type.hpp"
#include "semantic/expression.hpp"
#include "semantic/symbol.hpp"
#include "semantic/scope.hpp"

namespace llang {
namespace semantic {

void Scope::addSymbol(Symbol* symbol) {
	// TODO: check conflict

	symbols[symbol->name] = symbol;
}

Symbol* Scope::lookup(const identifier_t& name) {
	symbol_map_t::iterator it = symbols.find(name);
	
	if (it == symbols.end()) {
		if (parent_)
			return parent_->lookup(name);
		else
			return 0;
	}

	return it->second;
}

} // namespace semantic
} // namespace llang
