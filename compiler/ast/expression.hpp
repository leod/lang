#ifndef LLANG_AST_EXPRESSION_HPP_INCLUDED
#define LLANG_AST_EXPRESSION_HPP_INCLUDED

#include <list>

#include "util/smart_ptr.hpp"
#include "common/number.hpp"
#include "common/identifier.hpp"
#include "ast/node.hpp"

namespace llang {
namespace ast {

class Type;
class Declaration;

class Expression : public Node {
protected:
	Expression(const Node::Tag tag, const Location& location)
		: Node(tag, location) {
	}
};

typedef shared_ptr<Expression> ExpressionPtr;

class BinaryExpression : public Expression {
public:
	enum Operation {
		ADD,
		SUB,
		MUL,
		DIV,
		ASSIGN,
		EQUALS
	};

	BinaryExpression(const Location& location, Operation operation,
	                 Expression* left, Expression* right)
		: Expression(Node::BINARY_EXPRESSION, location),
		  operation(operation), left(left), right(right) {
	}

	const Operation operation;
	scoped_ptr<Expression> left, right;
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

class LiteralStringExpression : public Expression {
public:
	LiteralStringExpression(const Location& location, const std::string& string)
		: Expression(Node::LITERAL_STRING_EXPRESSION, location),
		  string(string) {
	}

	const std::string string;
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

	scoped_ptr<Expression> condition;
	scoped_ptr<Expression> ifExpression;
	scoped_ptr<Expression> elseExpression;
};

class VoidExpression : public Expression {
public:
	VoidExpression(const Location& location)
		: Expression(Node::VOID_EXPRESSION, location) {
	}
};

class IdentifierExpression : public Expression {
public:
	IdentifierExpression(const Location& location, const identifier_t& name)
		: Expression(Node::IDENTIFIER_EXPRESSION, location),
		  name(name) {
	}

	const identifier_t name;
};

class CallExpression : public Expression {
public:
	typedef std::list<ExpressionPtr> argument_list_t;

	CallExpression(const Location& location, Expression* callee,
	               argument_list_t arguments)
		: Expression(Node::CALL_EXPRESSION, location),
		  callee(callee), arguments(arguments) {
	}
	
	scoped_ptr<Expression> callee;
	argument_list_t arguments;
};

class DeclarationExpression : public Expression {
public:
	DeclarationExpression(const Location& location, Declaration* declaration)
		: Expression(Node::DECLARATION_EXPRESSION, location),
		  declaration(declaration) {
	}

	scoped_ptr<Declaration> declaration;	
};

} // namespace ast
} // namespace llang

#endif
