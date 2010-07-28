#ifndef LLANG_AST_EXPRESSION_HPP_INCLUDED
#define LLANG_AST_EXPRESSION_HPP_INCLUDED

#include <list>
#include <boost/shared_ptr.hpp>

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

class LiteralNumberExpression : public Expression {
public:
	typedef int_t Number;

	LiteralNumberExpression(const Location& location, const Number& number)
		: Expression(Node::LITERAL_NUMBER_EXPRESSION, location),
		  number_(number) {
	}

	const Number& number() const { return number_; }

private:
	const Number number_;
};

class BlockExpression : public Expression {
public:
	typedef std::list<ExpressionPtr> expression_list_t;

	BlockExpression(const Location& location,
	                const expression_list_t& expressions)
		: Expression(Node::BLOCK_EXPRESSION, location),
		  expressions_(expressions) {
	}

	const expression_list_t& expressions() const { return expressions_; }

private:
	const expression_list_t expressions_;
};

class IfElseExpression : public Expression {
public:
	IfElseExpression(const Location& location,
	                 Expression* condition,
	                 Expression* ifExpression,
	                 Expression* elseExpression)
		: Expression(Node::IF_ELSE_EXPRESSION, location),
		  condition_(condition),
		  ifExpression_(ifExpression),
		  elseExpression_(elseExpression) {
	}

	const Expression* condition() const { return condition_; }
	const Expression* ifExpression() const { return ifExpression_; }
	const Expression* elseExpression() const { return elseExpression_; }

private:
	Expression* condition_;
	Expression* ifExpression_;
	Expression* elseExpression_;
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
