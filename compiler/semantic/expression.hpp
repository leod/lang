#ifndef LLANG_SEMANTIC_EXPRESSION_HPP_INCLUDED
#define LLANG_SEMANTIC_EXPRESSION_HPP_INCLUDED

#include <list>
#include <cassert>
#include <boost/shared_ptr.hpp>

#include "ast/node.hpp"
#include "semantic/node.hpp"
#include "semantic/type_ptr.hpp"
#include "semantic/symbol_ptr.hpp"
#include "semantic/expression_ptr.hpp"

namespace llang {
namespace semantic {

class Expression : public Node {
public:
	TypePtr type;

protected:
	Expression(const Node::Tag tag, const ast::Node& astNode,
	           TypePtr type)
		: Node(tag, astNode), type(type) {
	}
};

typedef boost::shared_ptr<Expression> ExpressionPtr;

class BinaryExpression : public Expression {
public:
	BinaryExpression(const ast::Node& astNode, TypePtr type,
	                 ExpressionPtr left, ExpressionPtr right)
		: Expression(Node::BINARY_EXPRESSION, astNode, type),
		  left(left), right(right) {
	}

	ExpressionPtr left, right;
};

class LiteralNumberExpression : public Expression {
public:
	LiteralNumberExpression(const ast::Node& astNode, TypePtr type)
		: Expression(Node::LITERAL_NUMBER_EXPRESSION, astNode, type) {
	}
};

class BlockExpression : public Expression {
public:
	typedef std::list<ExpressionPtr> expression_list_t;

	BlockExpression(const ast::Node& astNode, TypePtr type,
	                const expression_list_t& expressions)
		: Expression(Node::BLOCK_EXPRESSION, astNode, type),
		  expressions(expressions) {
	}

	expression_list_t expressions;
};

class IfElseExpression : public Expression {
public:
	IfElseExpression(const ast::Node& astNode,
	                 TypePtr type,
	                 ExpressionPtr condition,
	                 ExpressionPtr ifExpression,
	                 ExpressionPtr elseExpression)
		: Expression(Node::IF_ELSE_EXPRESSION, astNode, type),
		  condition(condition),
		  ifExpression(ifExpression),
		  elseExpression(elseExpression) {
	}

	ExpressionPtr condition;
	ExpressionPtr ifExpression;
	ExpressionPtr elseExpression;
};

class VoidExpression : public Expression {
public:
	VoidExpression(const ast::Node& astNode, TypePtr type)
		: Expression(Node::VOID_EXPRESSION, astNode, type) {
	}
};

class SymbolExpression : public Expression {
public:
	SymbolExpression(const ast::Node& astNode, TypePtr type, 
	                 SymbolPtr symbol)
		: Expression(Node::IDENTIFIER_EXPRESSION, astNode, type),
		  symbol(symbol) {
		assert(symbol);
	}

	SymbolPtr symbol;
};

class CallExpression : public Expression {
public:
	typedef std::list<ExpressionPtr> argument_list_t;

	CallExpression(const ast::Node& astNode, TypePtr type, ExpressionPtr callee,
	               argument_list_t arguments)
		: Expression(Node::CALL_EXPRESSION, astNode, type),
		  callee(callee), arguments(arguments) {
	}
	
	ExpressionPtr callee;
	argument_list_t arguments;
};

} // namespace ast
} // namespace llang

#endif
