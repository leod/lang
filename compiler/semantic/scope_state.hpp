#ifndef LLANG_SEMANTIC_SCOPE_STATE_HPP_INCLUDED
#define LLANG_SEMANTIC_SCOPE_STATE_HPP_INCLUDED

#include "semantic/scope.hpp"

namespace llang {
namespace semantic {

struct ScopeState {
	Scope* scope;

	ScopeState() : scope(0) {}

	ScopeState withScope(Scope* scope) const {
		ScopeState result = *this;
		result.scope = scope;

		return result;
	}
};

} // namespace ast
} // namespace llang

#endif
