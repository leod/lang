#ifndef LLANG_SEMANTIC_EXPRESSION_HPP_INCLUDED
#define LLANG_SEMANTIC_EXPRESSION_HPP_INCLUDED

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "common/identifier.hpp"
#include "ast/node.hpp"
#include "semantic/node.hpp"

namespace llang {
namespace semantic {

class Type;

class Expression : public Node {
public:
	boost::scoped_ptr<Type> type;

protected:
	Expression(const Node::Tag tag, const ast::Node& astNode,
	           Type* type)
		: Node(tag, astNode), type(type) {
	}
};

typedef boost::shared_ptr<Expression> ExpressionPtr;

class BinaryExpression : public Expression {
public:
	BinaryExpression(const ast::Node& astNode, Type* type,
	                 Expression* left, Expression* right)
		: Expression(Node::BINARY_EXPRESSION, astNode, type),
		  left(left), right(right) {
	}

	boost::scoped_ptr<Expression> left, right;
};

class LiteralNumberExpression : public Expression {
public:
	LiteralNumberExpression(const ast::Node& astNode, Type* type)
		: Expression(Node::LITERAL_NUMBER_EXPRESSION, astNode, type) {
	}
};

class BlockExpression : public Expression {
public:
	typedef std::list<ExpressionPtr> expression_list_t;

	BlockExpression(const ast::Node& astNode, Type* type,
	                const expression_list_t& expressions)
		: Expression(Node::BLOCK_EXPRESSION, astNode, type),
		  expressions(expressions) {
	}

	expression_list_t expressions;
};

class IfElseExpression : public Expression {
public:
	IfElseExpression(const ast::Node& astNode,
	                 Type* type,
	                 Expression* condition,
	                 Expression* ifExpression,
	                 Expression* elseExpression)
		: Expression(Node::IF_ELSE_EXPRESSION, astNode, type),
		  condition(condition),
		  ifExpression(ifExpression),
		  elseExpression(elseExpression) {
	}

	boost::scoped_ptr<Expression> condition;
	boost::scoped_ptr<Expression> ifExpression;
	boost::scoped_ptr<Expression> elseExpression;
};

class VoidExpression : public Expression {
public:
	VoidExpression(const ast::Node& astNode, Type* type)
		: Expression(Node::VOID_EXPRESSION, astNode, type) {
	}
};

class IdentifierExpression : public Expression {
public:
	IdentifierExpression(const ast::Node& astNode, Type* type, 
	                     const identifier_t& name)
		: Expression(Node::IDENTIFIER_EXPRESSION, astNode, type),
		  name(name) {
	}

	const identifier_t name;
};

class CallExpression : public Expression {
public:
	typedef std::list<ExpressionPtr> argument_list_t;

	CallExpression(const ast::Node& astNode, Type* type, Expression* callee,
	               argument_list_t arguments)
		: Expression(Node::CALL_EXPRESSION, astNode, type),
		  callee(callee), arguments(arguments) {
	}
	
	boost::scoped_ptr<Expression> callee;
	argument_list_t arguments;
};

} // namespace ast
} // namespace llang

#endif
