#ifndef LLANG_SEMANTIC_AST_VISITOR_HPP_INCLUDED
#define LLANG_SEMANTIC_AST_VISITOR_HPP_INCLUDED

#include <list>
#include <boost/scoped_ptr.hpp>

#include "common/context.hpp"
#include "ast/node.hpp"
#include "ast/visitor.hpp"
#include "semantic/node.hpp"
#include "semantic/expression.hpp"
#include "semantic/type.hpp"
#include "semantic/symbol.hpp"
#include "semantic/scope_state.hpp"

namespace llang {
namespace semantic {

class AstVisitors {
public:
	boost::scoped_ptr<ast::Visitor<ScopeState, Type*> > typeVisitor;
	boost::scoped_ptr<ast::Visitor<ScopeState, Symbol*> > declarationVisitor;
	boost::scoped_ptr<ast::Visitor<ScopeState, Expression*> > expressionVisitor;

	AstVisitors(ast::Visitor<ScopeState, Type*>* typeVisitor,
	            ast::Visitor<ScopeState, Symbol*>* declarationVisitor,
	            ast::Visitor<ScopeState, Expression*>* expressionVisitor)
		: typeVisitor(typeVisitor),
		  declarationVisitor(declarationVisitor),
		  expressionVisitor(expressionVisitor) {
	}
};

AstVisitors* makeAstVisitors(Context&);

} // namespace ast
} // namespace llang

#endif
