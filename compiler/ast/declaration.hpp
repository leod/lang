#ifndef LLANG_AST_DECLARATION_HPP_INCLUDED
#define LLANG_AST_DECLARATION_HPP_INCLUDED

#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "common/identifier.hpp"
#include "ast/node.hpp"

namespace llang {
namespace ast {

class Type;
class Expression;

class Declaration : public Node {
protected:
	Declaration(const Node::Tag tag, const Location& location)
		: Node(tag, location) {
	}
};

typedef boost::shared_ptr<Declaration> DeclarationPtr;

class Module : public Declaration {
public:
	typedef std::list<DeclarationPtr> declaration_list_t;

	Module(const Location& location, const identifier_t& name,
	       declaration_list_t declarations)
		: Declaration(Node::MODULE, location),
		  name(name), declarations(declarations) {
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
		          const identifier_t& name);
		~Parameter();
	};

	typedef std::list<Parameter> parameter_list_t;

	FunctionDeclaration(const Location& location,
	                    Type* returnType,
	                    const identifier_t& name,
	                    parameter_list_t& parameters,
	                    Expression* body);

	virtual ~FunctionDeclaration();

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
	                    Expression* initializer);

	virtual ~VariableDeclaration();

	boost::scoped_ptr<Type> type;
	const identifier_t name;
	boost::scoped_ptr<Expression> initializer;
};

} // namespace ast
} // namespace llang

#endif
