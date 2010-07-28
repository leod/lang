#ifndef LLANG_SEMANTIC_SCOPE_HPP_INCLUDED
#define LLANG_SEMANTIC_SCOPE_HPP_INCLUDED

#include <map>

namespace llang {
namespace semantic {

class Symbol;

class Scope {
public:

private:
	typedef std::map<identifier_t, boost::shared_ptr<Symbol> > symbol_map_t;

	Scope* parent;

	symbol_map_t symbols;
};

} // namespace semantic
} // namespace llang

#endif
