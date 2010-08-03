#ifndef LLANG_SEMANTIC_SEMANTIC_VISITOR_HPP_INCLUDED
#define LLANG_SEMANTIC_SEMANTIC_VISITOR_HPP_INCLUDED

#include <list>

#include "util/smart_ptr.hpp"
#include "common/context.hpp"
#include "semantic/visitor.hpp"
#include "semantic/expr_ptr.hpp"
#include "semantic/type_ptr.hpp"
#include "semantic/decl_ptr.hpp"
#include "semantic/scope_state.hpp"

namespace llang {
namespace semantic {

class SemanticVisitors {
public:
	typedef Visitor<ScopeState, TypePtr> TypeVisitor;
	typedef Visitor<ScopeState, DeclPtr> DeclVisitor;
	typedef Visitor<ScopeState, ExprPtr> ExprVisitor;

	scoped_ptr<TypeVisitor> typeVisitor;
	scoped_ptr<DeclVisitor> declVisitor;
	scoped_ptr<ExprVisitor> exprVisitor;

	SemanticVisitors(TypeVisitor* typeVisitor,
	                 DeclVisitor* declVisitor,
	                 ExprVisitor* exprVisitor)
		: typeVisitor(typeVisitor),
		  declVisitor(declVisitor),
		  exprVisitor(exprVisitor) {
	}
};

SemanticVisitors* makeSemanticVisitors(Context&);

} // namespace semantic
} // namespace llang

#endif

