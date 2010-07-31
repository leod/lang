#ifndef LLANG_SEMANTIC_AST_VISITOR_HPP_INCLUDED
#define LLANG_SEMANTIC_AST_VISITOR_HPP_INCLUDED

#include <list>

#include "util/smart_ptr.hpp"
#include "common/context.hpp"
#include "ast/visitor.hpp"
#include "semantic/expression_ptr.hpp"
#include "semantic/type_ptr.hpp"
#include "semantic/symbol_ptr.hpp"
#include "semantic/scope_state.hpp"

namespace llang {
namespace semantic {

class AstVisitors {
public:
	typedef ast::Visitor<ScopeState, TypePtr> TypeVisitor;
	typedef ast::Visitor<ScopeState, SymbolPtr> DeclarationVisitor;
	typedef ast::Visitor<ScopeState, ExpressionPtr> ExpressionVisitor;

	scoped_ptr<TypeVisitor> typeVisitor;
	scoped_ptr<DeclarationVisitor> declarationVisitor;
	scoped_ptr<ExpressionVisitor> expressionVisitor;

	AstVisitors(TypeVisitor* typeVisitor,
	            DeclarationVisitor* declarationVisitor,
	            ExpressionVisitor* expressionVisitor)
		: typeVisitor(typeVisitor),
		  declarationVisitor(declarationVisitor),
		  expressionVisitor(expressionVisitor) {
	}
};

AstVisitors* makeAstVisitors(Context&);

} // namespace ast
} // namespace llang

#endif
