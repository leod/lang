#ifndef LLANG_SEMANTIC_SYMBOL_HPP_INCLUDED
#define LLANG_SEMANTIC_SYMBOL_HPP_INCLUDED

#include "ast/declaration.hpp"
#include "semantic/node.hpp"
#include "semantic/scope.hpp"

namespace llang {
namespace semantic {

class Type;
class Expression;

class Symbol : public Node {
public:
	const identifier_t name;
	Scope* declarationScope;

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
	             const identifier_t& name, Scope* declarationScope)
		: Symbol(tag, astNode, name, declarationScope),
		  scope(new Scope(declarationScope)) {
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
		boost::shared_ptr<Type> type;
		ParameterSymbol* symbol;

		Parameter(Type* type, 
		          ParameterSymbol* symbol)
			: type(type), symbol(symbol) {
		}
	};

	typedef std::list<Parameter> parameter_list_t;

	FunctionSymbol(const ast::FunctionDeclaration& astNode,
				   Scope* declarationScope,
	               Type* returnType,
	               parameter_list_t& parameters,
	               Expression* body)
		: Symbol(Node::FUNCTION_SYMBOL, astNode,
		         astNode.name, declarationScope),
		  returnType(returnType),
		  parameters(parameters),
		  body(body) {
	}

	boost::scoped_ptr<Type> returnType;
	parameter_list_t parameters;	
	boost::scoped_ptr<Expression> body;	
};

class VariableSymbol : public Symbol {
public:
	VariableSymbol(const ast::VariableDeclaration& astNode,
				   Scope* declarationScope,
	               Type* type,
	               Expression* initializer)
		: Symbol(Node::VARIABLE_SYMBOL, astNode,
		         astNode.name, declarationScope),
		  type(type),
		  initializer(initializer) {
	}

	boost::scoped_ptr<Type> type;
	boost::scoped_ptr<Expression> initializer;
};

class ParameterSymbol : public Symbol {
public:
	ParameterSymbol(const ast::Node& astNode,
	                const identifier_t& name, // TMP until I add ast::Param
	                Scope* declarationScope,
	                Type* type)
		: Symbol(Node::PARAMETER_SYMBOL, astNode, name,
		         declarationScope),
		  type(type) {
	}

	boost::scoped_ptr<Type> type;
	//boost::scoped_ptr<Expression> defaultValue;
};

} // namespace semantic
} // namespace llang

#endif
