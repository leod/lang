#ifndef LLANG_SEMANTIC_AST_VISITOR_HPP_INCLUDED
#define LLANG_SEMANTIC_AST_VISITOR_HPP_INCLUDED

#include <list>
#include <boost/scoped_ptr.hpp>

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
	boost::scoped_ptr<ast::Visitor<ScopeState, TypePtr> > typeVisitor;
	boost::scoped_ptr<ast::Visitor<ScopeState, SymbolPtr> > declarationVisitor;
	boost::scoped_ptr<ast::Visitor<ScopeState, ExpressionPtr> > expressionVisitor;

	AstVisitors(ast::Visitor<ScopeState, TypePtr>* typeVisitor,
	            ast::Visitor<ScopeState, SymbolPtr>* declarationVisitor,
	            ast::Visitor<ScopeState, ExpressionPtr>* expressionVisitor)
		: typeVisitor(typeVisitor),
		  declarationVisitor(declarationVisitor),
		  expressionVisitor(expressionVisitor) {
	}
};

AstVisitors* makeAstVisitors(Context&);

} // namespace ast
} // namespace llang

#endif
