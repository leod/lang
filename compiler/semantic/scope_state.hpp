#ifndef LLANG_SEMANTIC_SCOPE_STATE_HPP_INCLUDED
#define LLANG_SEMANTIC_SCOPE_STATE_HPP_INCLUDED

#include "ast/type_ptr.hpp"
#include "semantic/scope.hpp"

namespace llang {
namespace semantic {

struct ScopeState {
	Scope* scope;
	ast::TypePtr expectedType;
	
	// Null if we're not in a function
	ast::FunctionDeclPtr function;
	bool inNestedFunction;

	ScopeState()
		: scope(0), inNestedFunction(false) {
	}

	ScopeState withScope(Scope* scope) const {
		ScopeState result = *this;
		result.scope = scope;

		return result;
	}

	ScopeState withExpectedType(ast::TypePtr expectedType) const {
		ScopeState result = *this;
		result.expectedType = expectedType;

		return result;
	}
};

} // namespace ast
} // namespace llang

#endif
