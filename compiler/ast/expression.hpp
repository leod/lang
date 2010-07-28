#ifndef LLANG_AST_EXPRESSION_HPP_INCLUDED
#define LLANG_AST_EXPRESSION_HPP_INCLUDED

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "ast/node.hpp"

namespace llang {
namespace ast {

class Type;

class Expression : public Node {
public:
	//virtual const Type* type() const = 0;

protected:
	Expression(const Node::Tag tag, const Location& location)
		: Node(tag, location) {
	}
};

typedef boost::shared_ptr<Expression> ExpressionPtr;

class BinaryExpression : public Expression {
public:
	BinaryExpression(const Location& location, lexer::Token::Type operation,
	                 Expression* left, Expression* right)
		: Expression(Node::BINARY_EXPRESSION, location),
		  operation(operation), left(left), right(right) {
	}

	// TODO: I need to think about whether I want to
	//       do it like this for every node
	const lexer::Token::Type operation;
	boost::scoped_ptr<Expression> left, right;
};

class LiteralNumberExpression : public Expression {
public:
	typedef int_t Number;

	LiteralNumberExpression(const Location& location, const Number& number)
		: Expression(Node::LITERAL_NUMBER_EXPRESSION, location),
		  number(number) {
	}

	const Number number;
};

class BlockExpression : public Expression {
public:
	typedef std::list<ExpressionPtr> expression_list_t;

	BlockExpression(const Location& location,
	                const expression_list_t& expressions)
		: Expression(Node::BLOCK_EXPRESSION, location),
		  expressions(expressions) {
	}

	expression_list_t expressions;
};

class IfElseExpression : public Expression {
public:
	IfElseExpression(const Location& location,
	                 Expression* condition,
	                 Expression* ifExpression,
	                 Expression* elseExpression)
		: Expression(Node::IF_ELSE_EXPRESSION, location),
		  condition(condition),
		  ifExpression(ifExpression),
		  elseExpression(elseExpression) {
	}

	Expression* condition;
	Expression* ifExpression;
	Expression* elseExpression;
};

class VoidExpression : public Expression {
public:
	VoidExpression(const Location& location)
		: Expression(Node::VOID_EXPRESSION, location) {
	}
};

} // namespace ast
} // namespace llang

#endif
