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

// Used only internally
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
};

typedef shared_ptr<VariableDecl> VariableDeclPtr;

class ParameterDecl : public VariableDecl {
public:
	ParameterDecl(const Location& location,
	              const identifier_t& name,
				  TypePtr type)
		: VariableDecl(location, name, type, ExprPtr(), Node::PARAMETER_DECL) {
	}
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
		  body(body) {
	}

	std::string mangle() {
		// TODO

		return name;	
	}

	TypePtr returnType;
	ParameterList parameters;
	ExprPtr body;	
	
	TypePtr type;
};

typedef shared_ptr<FunctionDecl> FunctionDeclPtr;


} // namespace ast
} // namespace llang

#endif
