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

} // namespace semantic
} // namespace llang
