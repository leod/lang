#ifndef LLANG_AST_DECL_HPP_INCLUDED
#define LLANG_AST_DECL_HPP_INCLUDED

#include <list>

#include "util/smart_ptr.hpp"

#include "ast/node.hpp"
#include "ast/type_ptr.hpp"
#include "ast/decl_ptr.hpp"
#include "ast/expr_ptr.hpp"

#include "semantic/scope.hpp"

namespace llang {
namespace ast {

class Expr;

class Decl : public Node {
public:
	const identifier_t name;

	semantic::Scope* declScope;

protected:
	Decl(Node::Tag tag, const Location& location, const identifier_t& name)
		: Node(tag, location), name(name), declScope(0) {
	}
};

// Used internally
class DelayedDecl : public Decl {
public:
	DelayedDecl(const Location& location, const identifier_t& name)
		: Decl(Node::DELAYED_DECL, location, name) {
	}
};

typedef shared_ptr<DelayedDecl> DelayedDeclPtr;

class ScopedDecl : public Decl {
public:
	shared_ptr<semantic::Scope> scope;

protected:
	ScopedDecl(Node::Tag tag, const Location& location,
	           const identifier_t& name)
		: Decl(tag, location, name) {
	}
};

typedef shared_ptr<ScopedDecl> ScopedDeclPtr;

class Module : public ScopedDecl {
public:
	typedef std::list<DeclPtr> DeclList;

	Module(const Location& location, const identifier_t& name, DeclList& decls)
		: ScopedDecl(Node::MODULE, location, name),
		  decls(decls) {
	}

	DeclList decls;
};

typedef shared_ptr<Module> ModulePtr;

class FunctionDecl;
class VariableDecl : public Decl {
public:
	VariableDecl(const Location& location,
	             const identifier_t& name,
	             TypePtr type,
	             ExprPtr initializer,
	             Node::Tag tag = Node::VARIABLE_DECL)
		: Decl(tag, location, name),
		  type(type),
		  initializer(initializer) {
	}

	TypePtr type;
	ExprPtr initializer;

	// Null if not declared in a function
	shared_ptr<FunctionDecl> function;
};

typedef shared_ptr<VariableDecl> VariableDeclPtr;

class ParameterDecl : public VariableDecl {
public:
	ParameterDecl(const Location& location,
	              const identifier_t& name,
				  bool hasName,
	              TypePtr type)
		: VariableDecl(location, name, type, ExprPtr(), Node::PARAMETER_DECL),
		  hasName(hasName) {
	}

	bool hasName;
};

typedef shared_ptr<ParameterDecl> ParameterDeclPtr;

class FunctionDecl : public ScopedDecl {
public:
	typedef std::list<ParameterDeclPtr> ParameterList;

	FunctionDecl(const Location& location,
	             const identifier_t& name,
	             TypePtr returnType,
	             ParameterList& parameters,
	             ExprPtr body)
		: ScopedDecl(Node::FUNCTION_DECL, location, name),
		  returnType(returnType),
		  parameters(parameters),
		  body(body),
		  isExtern(false),
		  isNested(false) {
	}

	std::string mangle() {
		// TODO
		if (isNested)
			return parentFunction->mangle() + '_' + name;

		return name;	
	}

	TypePtr returnType;
	ParameterList parameters;
	ExprPtr body;	
	
	TypePtr type;

	bool isExtern;

	// A list of variables declared in outer functions used in this function.
	// This is used for nested functions.
	std::list<VariableDeclPtr> outerVariables;

	bool isNested;
	shared_ptr<FunctionDecl> parentFunction;
};

typedef shared_ptr<FunctionDecl> FunctionDeclPtr;

} // namespace ast
} // namespace llang

#endif
