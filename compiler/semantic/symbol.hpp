#ifndef LLANG_SEMANTIC_SYMBOL_HPP_INCLUDED
#define LLANG_SEMANTIC_SYMBOL_HPP_INCLUDED

#include "util/smart_ptr.hpp"

#include "ast/declaration.hpp"
#include "semantic/node.hpp"
#include "semantic/scope.hpp"
#include "semantic/type_ptr.hpp"
#include "semantic/symbol_ptr.hpp"
#include "semantic/expression_ptr.hpp"

namespace llang {
namespace semantic {

class Expression;

class Symbol : public Node {
public:
	const identifier_t name;
	Scope* const declarationScope;

protected:
	Symbol(Node::Tag tag, const ast::Node& astNode,
	       const identifier_t& name, Scope* declarationScope)
		: Node(tag, astNode), name(name),
		  declarationScope(declarationScope) {
	}
};

class ScopedSymbol : public Symbol {
public:
	scoped_ptr<Scope> scope;

protected:
	ScopedSymbol(Node::Tag tag, const ast::Node& astNode,
	             const identifier_t& name, Scope* scope)
		: Symbol(tag, astNode, name, scope->parent()),
		  scope(scope) {
	}
};

typedef shared_ptr<ScopedSymbol> ScopedSymbolPtr;

class Module : public ScopedSymbol {
public:
	Module(const ast::Module& astNode, Scope* scope)
		: ScopedSymbol(Node::MODULE, astNode, astNode.name, scope) {
	}
};

typedef shared_ptr<Module> ModulePtr;

class ParameterSymbol;
typedef shared_ptr<ParameterSymbol> ParameterSymbolPtr;

class FunctionSymbol : public ScopedSymbol {
public:
	struct Parameter {
		TypePtr type;
		ParameterSymbolPtr symbol;

		Parameter(TypePtr type, ParameterSymbolPtr symbol)
			: type(type), symbol(symbol) {
		}
	};

	typedef std::list<Parameter> parameter_list_t;

	FunctionSymbol(const ast::FunctionDeclaration& astNode,
	               Scope* scope,
	               TypePtr returnType,
	               parameter_list_t& parameters,
	               ExpressionPtr body,
	               TypePtr type)
		: ScopedSymbol(Node::FUNCTION_SYMBOL, astNode,
		               astNode.name, scope),
		  returnType(returnType),
		  parameters(parameters),
		  body(body),
		  type(type) {
	}

	TypePtr returnType;
	parameter_list_t parameters;	
	ExpressionPtr body;	
	
	TypePtr type;
};

typedef shared_ptr<FunctionSymbol> FunctionSymbolPtr;

class VariableSymbol : public Symbol {
public:
	VariableSymbol(const ast::VariableDeclaration& astNode,
	               Scope* declarationScope,
	               TypePtr type,
	               ExpressionPtr initializer)
		: Symbol(Node::VARIABLE_SYMBOL, astNode,
		         astNode.name, declarationScope),
		  type(type),
		  initializer(initializer) {
	}

	TypePtr type;
	ExpressionPtr initializer;
};

typedef shared_ptr<VariableSymbol> VariableSymbolPtr;

class ParameterSymbol : public Symbol {
public:
	ParameterSymbol(const ast::Node& astNode,
	                const identifier_t& name, // TMP until I add ast::Param
	                Scope* declarationScope,
	                TypePtr type)
		: Symbol(Node::PARAMETER_SYMBOL, astNode, name,
		         declarationScope),
		  type(type) {
	}

	TypePtr type;
};

} // namespace semantic
} // namespace llang

#endif
