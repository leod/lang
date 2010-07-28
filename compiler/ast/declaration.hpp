#ifndef LLANG_AST_DECLARATION_HPP_INCLUDED
#define LLANG_AST_DECLARATION_HPP_INCLUDED

#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "common/identifier.hpp"
#include "ast/node.hpp"
#include "ast/type.hpp"
#include "ast/expression.hpp"

namespace llang {
namespace ast {

class Declaration : public Node {
protected:
	Declaration(const Node::Tag tag, const Location& location)
		: Node(tag, location) {
	}
};

class Module : public Declaration {
public:
	typedef std::list<boost::shared_ptr<Declaration> > declaration_list_t;

	Module(const Location& location, const identifier_t& name)
		: Declaration(Node::MODULE, location),
		  name(name) {
	}

	void addDeclaration(Declaration* declaration) {
		declarations.push_back(boost::shared_ptr<Declaration>(declaration));
	}

	const identifier_t name;	
	declaration_list_t declarations;
};

class FunctionDeclaration : public Declaration {
public:
	struct Parameter {
		boost::shared_ptr<Type> type; // need shared_ptr for std::list
		const bool hasName;
		const identifier_t name;

		Parameter(Type* type, 
		          bool hasName,
		          const identifier_t& name)
			: type(type), hasName(hasName), name(name) {
		}
	};

	typedef std::list<Parameter> parameter_list_t;

	FunctionDeclaration(const Location& location,
	                    Type* returnType,
	                    const identifier_t& name,
	                    parameter_list_t& parameters,
						Expression* body)
		: Declaration(Node::FUNCTION_DECLARATION, location),
		  returnType(returnType),
		  name(name),
		  parameters(parameters),
		  body(body) {
	}

	boost::scoped_ptr<Type> returnType;
	const identifier_t name;
	parameter_list_t parameters;	
	boost::scoped_ptr<Expression> body;	
};

class VariableDeclaration : public Declaration {
public:
	VariableDeclaration(const Location& location,
	                    Type* type,
	                    const identifier_t& name,
	                    Expression* initializer)
		: Declaration(Node::VARIABLE_DECLARATION, location),
		  type(type),
		  name(name),
		  initializer(initializer) {
	}

	boost::scoped_ptr<Type> type;
	const identifier_t name;
	boost::scoped_ptr<Expression> initializer;
};

} // namespace ast
} // namespace llang

#endif
