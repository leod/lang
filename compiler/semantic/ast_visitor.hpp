#ifndef LLANG_SEMANTIC_AST_VISITOR_HPP_INCLUDED
#define LLANG_SEMANTIC_AST_VISITOR_HPP_INCLUDED

#include <list>

#include "util/smart_ptr.hpp"
#include "common/context.hpp"
#include "ast/visitor.hpp"
#include "semantic/expr_ptr.hpp"
#include "semantic/type_ptr.hpp"
#include "semantic/decl_ptr.hpp"
#include "semantic/scope_state.hpp"

namespace llang {
namespace semantic {

class AstVisitors {
public:
	typedef ast::Visitor<ScopeState, TypePtr> TypeVisitor;
	typedef ast::Visitor<ScopeState, DeclPtr> DeclVisitor;
	typedef ast::Visitor<ScopeState, ExprPtr> ExprVisitor;

	scoped_ptr<TypeVisitor> typeVisitor;
	scoped_ptr<DeclVisitor> declVisitor;
	scoped_ptr<ExprVisitor> exprVisitor;

	AstVisitors(TypeVisitor* typeVisitor,
	            DeclVisitor* declVisitor,
	            ExprVisitor* exprVisitor)
		: typeVisitor(typeVisitor),
		  declVisitor(declVisitor),
		  exprVisitor(exprVisitor) {
	}
};

AstVisitors* makeAstVisitors(Context&);

} // namespace semantic
} // namespace llang

#endif
