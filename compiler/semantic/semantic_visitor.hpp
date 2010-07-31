#ifndef LLANG_SEMANTIC_SEMANTIC_VISITOR_HPP_INCLUDED
#define LLANG_SEMANTIC_SEMANTIC_VISITOR_HPP_INCLUDED

#include <list>

#include "util/smart_ptr.hpp"
#include "common/context.hpp"
#include "semantic/visitor.hpp"
#include "semantic/expression_ptr.hpp"
#include "semantic/type_ptr.hpp"
#include "semantic/symbol_ptr.hpp"
#include "semantic/scope_state.hpp"

namespace llang {
namespace semantic {

class SemanticVisitors {
public:
	typedef Visitor<ScopeState, TypePtr> TypeVisitor;
	typedef Visitor<ScopeState, SymbolPtr> SymbolVisitor;
	typedef Visitor<ScopeState, ExpressionPtr> ExpressionVisitor;

	scoped_ptr<TypeVisitor> typeVisitor;
	scoped_ptr<SymbolVisitor> symbolVisitor;
	scoped_ptr<ExpressionVisitor> expressionVisitor;

	SemanticVisitors(TypeVisitor* typeVisitor,
	                 SymbolVisitor* symbolVisitor,
	                 ExpressionVisitor* expressionVisitor)
		: typeVisitor(typeVisitor),
		  symbolVisitor(symbolVisitor),
		  expressionVisitor(expressionVisitor) {
	}
};

SemanticVisitors* makeSemanticVisitors(Context&);

} // namespace ast
} // namespace llang

#endif

