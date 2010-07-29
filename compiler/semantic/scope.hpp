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
	Scope(Scope* parent = 0) : parent(parent) {}

	void addSymbol(Symbol* symbol);

private:
	typedef std::map<identifier_t, Symbol*> symbol_map_t;

	Scope* parent;

	symbol_map_t symbols;
};

} // namespace semantic
} // namespace llang

#endif
