#ifndef LLANG_AST_EXPR_HPP_INCLUDED
#define LLANG_AST_EXPR_HPP_INCLUDED

#include <list>
#include <cassert>

#include "util/smart_ptr.hpp"
#include "common/number.hpp"
#include "ast/node.hpp"
#include "ast/type_ptr.hpp"
#include "ast/decl_ptr.hpp"
#include "ast/expr_ptr.hpp"
#include "semantic/scope.hpp"

namespace llang {
namespace ast {

class Expr : public Node {
public:
	TypePtr type;

protected:
	Expr(const Node::Tag tag, const Location& location)
		: Node(tag, location) {
	}
};

// Used only internally
class DelayedExpr : public Expr {
public:
	DelayedExpr(const Location& location, DeclPtr delayedDecl)
		: Expr(Node::DELAYED_EXPR, location),
		  delayedDecl(delayedDecl) {
	}
	
	DeclPtr delayedDecl;		
};

typedef shared_ptr<DelayedExpr> DelayedExprPtr;

class BinaryExpr : public Expr {
public:
	enum Operation {
		ADD,
		SUB,
		MUL,
		DIV,
		ASSIGN,
		EQUALS
	};

	BinaryExpr(const Location& location, Operation operation,
	           ExprPtr left, ExprPtr right)
		: Expr(Node::BINARY_EXPR, location),
		  operation(operation), left(left), right(right) {
	}

	const Operation operation;
	ExprPtr left, right;
};

typedef shared_ptr<BinaryExpr> BinaryExprPtr;

class LiteralNumberExpr : public Expr {
public:
	LiteralNumberExpr(const Location& location, int_t number)
		: Expr(Node::LITERAL_NUMBER_EXPR, location),
		  number(number) {
	}

	int_t number;
};

typedef shared_ptr<LiteralNumberExpr> LiteralNumberExprPtr;

class LiteralStringExpr : public Expr {
public:
	LiteralStringExpr(const Location& location, const std::string& string)
		: Expr(Node::LITERAL_STRING_EXPR, location),
		  string(string) {
	}

	std::string string;
};

typedef shared_ptr<LiteralStringExpr> LiteralStringExprPtr;

class LiteralBoolExpr : public Expr {
public:
	LiteralBoolExpr(const Location& location, bool value)
		: Expr(Node::LITERAL_BOOL_EXPR, location),
		  value(value) {
	}

	bool value;
};

typedef shared_ptr<LiteralBoolExpr> LiteralBoolExprPtr;

class BlockExpr : public Expr {
public:
	typedef std::list<ExprPtr> ExprList;

	BlockExpr(const Location& location, ExprList& exprs)
		: Expr(Node::BLOCK_EXPR, location),
	      exprs(exprs) {
	}

	ExprList exprs;

	shared_ptr<semantic::Scope> scope;
};

typedef shared_ptr<BlockExpr> BlockExprPtr;

class IfElseExpr : public Expr {
public:
	IfElseExpr(const Location& location, ExprPtr condition,
	           ExprPtr ifExpr, ExprPtr elseExpr)
		: Expr(Node::IF_ELSE_EXPR, location),
		  condition(condition),
		  ifExpr(ifExpr),
		  elseExpr(elseExpr) {
	}

	ExprPtr condition;
	ExprPtr ifExpr;
	ExprPtr elseExpr;
};

typedef shared_ptr<IfElseExpr> IfElseExprPtr;

class VoidExpr : public Expr {
public:
	VoidExpr(const Location& location)
		: Expr(Node::VOID_EXPR, location) {
	}
};

typedef shared_ptr<VoidExpr> VoidExprPtr;

class IdentifierExpr : public Expr {
public:
	IdentifierExpr(const Location& location, const identifier_t& name)
		: Expr(Node::IDENTIFIER_EXPR, location),
		  name(name) {
	}

	const identifier_t name;
};

typedef shared_ptr<IdentifierExpr> IdentifierExprPtr;

class DeclRefExpr : public Expr {
public:
	DeclRefExpr(const Location& location, TypePtr type, DeclPtr decl)
		: Expr(Node::DECL_REF_EXPR, location),
		  decl(decl) {
		Expr::type = type;
	}

	weak_ptr<Decl> decl;
};

typedef shared_ptr<DeclRefExpr> DeclRefExprPtr;

class CallExpr : public Expr {
public:
	typedef std::list<ExprPtr> ArgumentList;

	CallExpr(const Location& location, ExprPtr callee,
	         ArgumentList arguments)
		: Expr(Node::CALL_EXPR, location),
		  callee(callee), arguments(arguments) {
	}

	ExprPtr callee;
	ArgumentList arguments;
};

typedef shared_ptr<CallExpr> CallExprPtr;

class DeclExpr : public Expr {
public:
	DeclExpr(const Location& location, DeclPtr decl)
		: Expr(Node::DECL_EXPR, location),
		  decl(decl) {
	}

	shared_ptr<Decl> decl;
};

typedef shared_ptr<DeclExpr> DeclExprPtr;

} // namespace ast
} // namespace llang

#endif
