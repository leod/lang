#include <stdexcept>

#include "ast/decl.hpp"
#include "semantic/scope.hpp"

namespace llang {
namespace semantic {

using namespace ast;

void Scope::addDecl(DeclPtr decl) {
	assert(decl);
	assert(decl->name != "");

	if (decls.find(decl->name) != decls.end())
		throw std::runtime_error("symbol already declared: " + decl->name);

	decls.insert(DeclMap::value_type(decl->name, decl));
}

DeclPtr Scope::lookup(const identifier_t& name) {
	DeclMap::iterator it = decls.find(name);
	
	if (it == decls.end()) {
		if (parent_)
			return parent_->lookup(name);
		else
			return DeclPtr();
	}

	return it->second;
}

} // namespace semantic
} // namespace llang
