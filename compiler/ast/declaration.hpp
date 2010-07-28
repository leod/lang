#ifndef LLANG_AST_DECLARATION_HPP_INCLUDED
#define LLANG_AST_DECLARATION_HPP_INCLUDED

#include <list>

#include "boost/shared_ptr.hpp"

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

class Module : Declaration {
public:
	typedef std::list<boost::shared_ptr<Declaration> > declaration_list_t;

	Module(const Location& location, const identifier_t& name)
		: Declaration(Node::MODULE, location),
		  name_(name) {
	}

	const identifier_t& name() const { return name_; }
	const declaration_list_t& declarations() const { return declarations_; }

	void addDeclaration(Declaration* declaration) {
		declarations_.push_back(boost::shared_ptr<Declaration>(declaration));
	}

private:
	identifier_t name_;	
	declaration_list_t declarations_;
};

class FunctionDeclaration : public Declaration {
public:
	struct Parameter {
		const bool hasName;
		const identifier_t name;
		const boost::shared_ptr<Type> type; // need shared_ptr for std::list

		Parameter(bool hasName,
		          const identifier_t& name,
		          Type* type)
			: hasName(hasName),
			  name(name),
			  type(type) {
		}
	};

	typedef std::list<Parameter> parameter_list_t;

	FunctionDeclaration(const Location& location,
	                    const Type* returnType,
	                    const identifier_t& name,
	                    const parameter_list_t& parameters)
		: Declaration(Node::FUNCTION_DECLARATION, location),
		  returnType_(returnType),
		  name_(name),
		  parameters_(parameters) {
	}

	const Type* returnType() const { return returnType_; }
	const identifier_t name() const { return name_; }
	const parameter_list_t parameters() const { return parameters_; }

private:
	const Type* returnType_;
	const identifier_t name_;
	const parameter_list_t parameters_;	
};

class VariableDeclaration : public Declaration {
public:
	VariableDeclaration(const Location& location,
	                    const Type* type,
	                    const identifier_t& name,
	                    const Expression* initializer)
		: Declaration(Node::VARIABLE_DECLARATION, location),
		  type_(type),
		  name_(name),
		  initializer_(initializer) {
	}

	const Type* type() const { return type_; }
	const identifier_t name() const { return name_; }
	const Expression* initializer() const { return initializer_; }

private:
	const Type* type_;
	const identifier_t name_;
	const Expression* initializer_;
};

} // namespace ast
} // namespace llang

#endif
