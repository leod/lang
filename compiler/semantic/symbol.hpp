#ifndef LLANG_SEMANTIC_DECL_HPP_INCLUDED
#define LLANG_SEMANTIC_DECL_HPP_INCLUDED

#include "util/smart_ptr.hpp"

#include "ast/decl.hpp"
#include "semantic/node.hpp"
#include "semantic/scope.hpp"
#include "semantic/type_ptr.hpp"
#include "semantic/decl_ptr.hpp"
#include "semantic/expr_ptr.hpp"

namespace llang {
namespace semantic {

class Expr;

class Decl : public Node {
public:
	const identifier_t name;
	Scope* const declScope;

protected:
	Decl(Node::Tag tag, const ast::Node& astNode,
	       const identifier_t& name, Scope* declScope)
		: Node(tag, astNode), name(name),
		  declScope(declScope) {
	}
};

// Used only internally
class DelayedDecl : public Decl {
public:
	DelayedDecl(const ast::Node& astNode, const identifier_t& name,
	              Scope* declScope)
		: Decl(Node::DELAYED_DECL, astNode, name, declScope) {
	}
};

typedef shared_ptr<DelayedDecl> DelayedDeclPtr;

class ScopedDecl : public Decl {
public:
	scoped_ptr<Scope> scope;

protected:
	ScopedDecl(Node::Tag tag, const ast::Node& astNode,
	             const identifier_t& name, Scope* scope)
		: Decl(tag, astNode, name, scope->parent()),
		  scope(scope) {
	}
};

typedef shared_ptr<ScopedDecl> ScopedDeclPtr;

class Module : public ScopedDecl {
public:
	Module(const ast::Module& astNode, Scope* scope)
		: ScopedDecl(Node::MODULE, astNode, astNode.name, scope) {
	}
};

typedef shared_ptr<Module> ModulePtr;

class ParameterDecl;
typedef shared_ptr<ParameterDecl> ParameterDeclPtr;

class FunctionDecl : public ScopedDecl {
public:
	struct Parameter {
		TypePtr type;
		ParameterDeclPtr decl;

		Parameter(TypePtr type, ParameterDeclPtr decl)
			: type(type), decl(decl) {
		}
	};

	typedef std::list<Parameter> parameter_list_t;

	FunctionDecl(const ast::FunctionDecl& astNode,
	               Scope* scope,
	               TypePtr returnType,
	               parameter_list_t& parameters,
	               ExprPtr body,
	               TypePtr type)
		: ScopedDecl(Node::FUNCTION_DECL, astNode,
		               astNode.name, scope),
		  returnType(returnType),
		  parameters(parameters),
		  body(body),
		  type(type) {
	}

	std::string mangle() {
		// TODO

		return name;	
	}

	TypePtr returnType;
	parameter_list_t parameters;	
	ExprPtr body;	
	
	TypePtr type;
};

typedef shared_ptr<FunctionDecl> FunctionDeclPtr;

class VariableDecl : public Decl {
public:
	VariableDecl(const ast::VariableDecl& astNode,
	               Scope* declScope,
	               TypePtr type,
	               ExprPtr initializer)
		: Decl(Node::VARIABLE_DECL, astNode,
		         astNode.name, declScope),
		  type(type),
		  initializer(initializer) {
	}

	TypePtr type;
	ExprPtr initializer;
};

typedef shared_ptr<VariableDecl> VariableDeclPtr;

class ParameterDecl : public Decl {
public:
	ParameterDecl(const ast::Node& astNode,
	                const identifier_t& name, // TMP until I add ast::Param
	                Scope* declScope,
	                TypePtr type)
		: Decl(Node::PARAMETER_DECL, astNode, name,
		         declScope),
		  type(type) {
	}

	TypePtr type;
};

} // namespace semantic
} // namespace llang

#endif
