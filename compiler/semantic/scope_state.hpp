#ifndef LLANG_SEMANTIC_SCOPE_STATE_HPP_INCLUDED
#define LLANG_SEMANTIC_SCOPE_STATE_HPP_INCLUDED

#include "semantic/scope.hpp"

namespace llang {
namespace semantic {

struct ScopeState {
	Scope* scope;

	ScopeState() : scope(0) {}
};

} // namespace ast
} // namespace llang

#endif
