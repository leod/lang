#include <stdexcept>

#include "semantic/type.hpp"
#include "semantic/expression.hpp"
#include "semantic/symbol.hpp"
#include "semantic/scope.hpp"

namespace llang {
namespace semantic {

void Scope::addSymbol(Symbol* symbol) {
	assert(symbol);

	if (symbols.find(symbol->name) != symbols.end())
		throw std::runtime_error("symbol already declared: " + symbol->name);

	symbols[symbol->name] = SymbolPtr(symbol);
}

Symbol* Scope::lookup(const identifier_t& name) {
	symbol_map_t::iterator it = symbols.find(name);
	
	if (it == symbols.end()) {
		if (parent_)
			return parent_->lookup(name);
		else
			return 0;
	}

	return it->second.get();
}

} // namespace semantic
} // namespace llang
