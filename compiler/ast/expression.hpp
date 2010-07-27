#ifndef LLANG_AST_EXPRESSION_HPP_INCLUDED
#define LLANG_AST_EXPRESSION_HPP_INCLUDED

#include <list>
#include <boost/scoped_ptr.hpp>

#include "ast/node.hpp"

namespace llang {
namespace ast {

class Type;

class Expression : public Node {
public:
	virtual const Type* type() const = 0;

protected:
	explicit Expression(const Location& location)
		: Node(location) {
	}
};

template<typename Number> class LiteralNumberExpression : public Expression {
public:
	LiteralNumberExpression(const Location& location, const Number& number)
		: number_(number) {
	}

	const Number& number() const { return number_; }

private:
	const Number number_;
};

template<typename T> class BlockExpression : public Expression {
public:
	typedef std::list<boost::scoped_ptr<Expression> > expression_list_t;

	BlockExpression(const Location& location)
		: Expression(location) {
	}

	const expression_list_t& expressions() const { return expressions; }

private:
	expression_list_t expressions_;
};

class IfElseExpression : public Expression {
public:
	IfElseExpression(const Location& location,
	                 Expression* condition,
	                 Expression* ifExpression,
	                 Expression* elseExpression)
		: Expression(location),
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
		: Expression(location) {
	}
};

} // namespace ast
} // namespace llang

#endif
