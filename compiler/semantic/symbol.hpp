#ifndef LLANG_SEMANTIC_SYMBOL_HPP_INCLUDED
#define LLANG_SEMANTIC_SYMBOL_HPP_INCLUDED

#include "ast/declaration.hpp"
#include "semantic/node.hpp"
#include "semantic/scope.hpp"
#include "semantic/type_ptr.hpp"

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

typedef boost::shared_ptr<Symbol> SymbolPtr;

class ScopedSymbol : public Symbol {
public:
	Scope* const scope;

protected:
	ScopedSymbol(Node::Tag tag, const ast::Node& astNode,
	             const identifier_t& name, Scope* scope)
		: Symbol(tag, astNode, name, scope->parent()),
		  scope(scope) {
	}
};

class Module : public ScopedSymbol {
public:
	Module(const ast::Module& astNode)
		: ScopedSymbol(Node::MODULE, astNode, astNode.name, 0) {
	}
};

class ParameterSymbol;
class FunctionSymbol : public Symbol {
public:
	struct Parameter {
		TypePtr type;
		ParameterSymbol* symbol;

		Parameter(TypePtr type, 
		          ParameterSymbol* symbol)
			: type(type), symbol(symbol) {
		}
	};

	typedef std::list<Parameter> parameter_list_t;

	FunctionSymbol(const ast::FunctionDeclaration& astNode,
	               Scope* declarationScope,
	               TypePtr returnType,
	               parameter_list_t& parameters,
	               Expression* body,
	               TypePtr type)
		: Symbol(Node::FUNCTION_SYMBOL, astNode,
		         astNode.name, declarationScope),
		  returnType(returnType),
		  parameters(parameters),
		  body(body),
		  type(type) {
	}

	TypePtr returnType;
	parameter_list_t parameters;	
	boost::scoped_ptr<Expression> body;	
	
	TypePtr type;
};

class VariableSymbol : public Symbol {
public:
	VariableSymbol(const ast::VariableDeclaration& astNode,
	               Scope* declarationScope,
	               TypePtr type,
	               Expression* initializer)
		: Symbol(Node::VARIABLE_SYMBOL, astNode,
		         astNode.name, declarationScope),
		  type(type),
		  initializer(initializer) {
	}

	TypePtr type;
	boost::scoped_ptr<Expression> initializer;
};

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
	//boost::scoped_ptr<Expression> defaultValue;
};

} // namespace semantic
} // namespace llang

#endif
