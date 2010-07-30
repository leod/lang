#ifndef LLANG_SEMANTIC_TYPE_HPP_INCLUDED
#define LLANG_SEMANTIC_TYPE_HPP_INCLUDED

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "lexer/token.hpp"
#include "ast/type.hpp"
#include "semantic/node.hpp"

namespace llang {
namespace semantic {

class Type : public Node {
public:
	// TODO: consider making all types singletons to make
	//       comparing them easier and faster
	virtual bool equals(const Type* other) const = 0;
	bool equals(const boost::scoped_ptr<Type>& other) {
		return equals(other.get());
	}
	bool equals(const boost::shared_ptr<Type>& other) {
		return equals(other.get());
	}

protected:
	Type(const Node::Tag tag, const ast::Node& astNode)
		: Node(tag, astNode) {
	}
};

typedef boost::shared_ptr<Type> TypePtr;

class IntegralType : public Type {
public:
	IntegralType(const ast::IntegralType& astNode)
		: Type(Node::INTEGRAL_TYPE, astNode),
		  type(astNode.type) {
	}

	virtual bool equals(const Type* other) const {
		const IntegralType* type = other->isA<IntegralType>();
		if (!type) return false;

		return type->type == this->type;
	}

	const lexer::Token::Type type;
};

class FunctionType : public Type {
public:
	typedef std::list<TypePtr> parameter_type_list_t;

	FunctionType(const ast::Node& astNode,
				 Type* returnType, 
	             parameter_type_list_t& parameterTypes)
		: Type(Node::FUNCTION_TYPE, astNode),
		  returnType(returnType),
		  parameterTypes(parameterTypes) {
	}

	virtual bool equals(const Type* other) const {
		const FunctionType* type = other->isA<FunctionType>();
		if (!type) return false;
		
		if (!type->returnType->equals(returnType)) return false;
		if (parameterTypes.size() != type->parameterTypes.size()) return false;

		for (auto it1 = parameterTypes.begin(),
		          it2 = type->parameterTypes.begin();
		     it1 != parameterTypes.end();
			 ++it1, ++it2) {
			if (!(*it1)->equals(*it2)) return false;
		}

		return true;
	}

	boost::scoped_ptr<Type> returnType;
	parameter_type_list_t parameterTypes;	
};

} // namespace ast
} // namespace llang

#endif
