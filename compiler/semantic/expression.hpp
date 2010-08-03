#ifndef LLANG_SEMANTIC_EXPRESSION_HPP_INCLUDED
#define LLANG_SEMANTIC_EXPRESSION_HPP_INCLUDED

#include <list>
#include <cassert>

#include "util/smart_ptr.hpp"
#include "ast/node.hpp"
#include "ast/expression.hpp"
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

// Used only internally
class DelayedExpression : public Expression {
public:
	DelayedExpression(const ast::Node& astNode, TypePtr type, 
	                  SymbolPtr delayedSymbol)
		: Expression(Node::DELAYED_EXPRESSION, astNode, type),
		  delayedSymbol(delayedSymbol) {
	}
	
	SymbolPtr delayedSymbol;		
};

typedef shared_ptr<DelayedExpression> DelayedExpressionPtr;

class BinaryExpression : public Expression {
public:
	BinaryExpression(const ast::BinaryExpression& astNode, TypePtr type,
	                 ExpressionPtr left, ExpressionPtr right)
		: Expression(Node::BINARY_EXPRESSION, astNode, type),
		  operation(astNode.operation), left(left), right(right) {
	}

	ast::BinaryExpression::Operation operation;
	ExpressionPtr left, right;
};

typedef shared_ptr<BinaryExpression> BinaryExpressionPtr;

class LiteralNumberExpression : public Expression {
public:
	LiteralNumberExpression(const ast::LiteralNumberExpression& astNode,
	                        TypePtr type)
		: Expression(Node::LITERAL_NUMBER_EXPRESSION, astNode, type),
		  number(astNode.number) {
	}

	int_t number;
};

typedef shared_ptr<LiteralNumberExpression> LiteralNumberExpressionPtr;

class LiteralStringExpression : public Expression {
public:
	LiteralStringExpression(const ast::Node& astNode, TypePtr type)
		: Expression(Node::LITERAL_STRING_EXPRESSION, astNode, type) {
	}
};

typedef shared_ptr<LiteralStringExpression> LiteralStringExpressionPtr;

class BlockExpression : public Expression {
public:
	typedef std::list<ExpressionPtr> expression_list_t;

	BlockExpression(const ast::Node& astNode, TypePtr type,
	                Scope* scope, const expression_list_t& expressions)
		: Expression(Node::BLOCK_EXPRESSION, astNode, type),
		  scope(scope), expressions(expressions) {
	}

	scoped_ptr<Scope> scope;
	expression_list_t expressions;
};

typedef shared_ptr<BlockExpression> BlockExpressionPtr;

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

typedef shared_ptr<IfElseExpression> IfElseExpressionPtr;

class VoidExpression : public Expression {
public:
	VoidExpression(const ast::Node& astNode, TypePtr type)
		: Expression(Node::VOID_EXPRESSION, astNode, type) {
	}
};

typedef shared_ptr<VoidExpression> VoidExpressionPtr;

class SymbolExpression : public Expression {
public:
	SymbolExpression(const ast::Node& astNode, TypePtr type, 
	                 SymbolPtr symbol)
		: Expression(Node::SYMBOL_EXPRESSION, astNode, type),
		  symbol(symbol) {
		assert(symbol);
	}

	weak_ptr<Symbol> symbol;
};

typedef shared_ptr<SymbolExpression> SymbolExpressionPtr;

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

typedef shared_ptr<CallExpression> CallExpressionPtr;

class DeclarationExpression : public Expression {
public:
	DeclarationExpression(const ast::Node& astNode, TypePtr type,
	                      SymbolPtr symbol)
		: Expression(Node::DECLARATION_EXPRESSION, astNode, type),
		  symbol(symbol) {
	}

	SymbolPtr symbol;
};

typedef shared_ptr<DeclarationExpression> DeclarationExpressionPtr;

} // namespace semantic
} // namespace llang

#endif
