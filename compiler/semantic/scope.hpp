#ifndef LLANG_SEMANTIC_SCOPE_HPP_INCLUDED
#define LLANG_SEMANTIC_SCOPE_HPP_INCLUDED

#include <map>

#include "util/smart_ptr.hpp"
#include "common/identifier.hpp"
#include "ast/decl_ptr.hpp"

namespace llang {
namespace semantic {

class Scope {
public:
	Scope(Scope* parent = 0) : parent_(parent) {}

	void addDecl(ast::DeclPtr decl);
	ast::DeclPtr lookup(const identifier_t& name);

	Scope* parent() { return parent_; }
	const Scope* parent() const { return parent_; }

	typedef std::map<identifier_t, ast::DeclPtr> DeclMap;
	DeclMap decls;

private:
	Scope* parent_;
};

typedef shared_ptr<Scope> ScopePtr;

} // namespace semantic
} // namespace llang

#endif
