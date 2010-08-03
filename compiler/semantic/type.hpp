#ifndef LLANG_SEMANTIC_TYPE_HPP_INCLUDED
#define LLANG_SEMANTIC_TYPE_HPP_INCLUDED

#include <list>
#include <sstream>
#include <string>

#include "lexer/token.hpp"
#include "ast/type.hpp"
#include "semantic/node.hpp"
#include "semantic/type_ptr.hpp"
#include "semantic/symbol_ptr.hpp"

namespace llang {
namespace semantic {

class Type : public Node {
public:
	// TODO: consider making all types singletons to make
	//       comparing them easier and faster
	virtual bool equals(const Type* other) const = 0;
	bool equals(const scoped_ptr<Type>& other) {
		return equals(other.get());
	}
	bool equals(const shared_ptr<Type>& other) {
		return equals(other.get());
	}

	virtual std::string name() const = 0;

protected:
	Type(const Node::Tag tag, const ast::Node& astNode)
		: Node(tag, astNode) {
	}
};

// Used only internally
class DelayedType : public Type {
public:
	DelayedType(const ast::Node& node, SymbolPtr delayedSymbol)
		: Type(Node::DELAYED_TYPE, node),
		  delayedSymbol(delayedSymbol) {
	}

	virtual bool equals(const Type*) const { return false; }	
	virtual std::string name() const { return "<undefined>"; }

	SymbolPtr delayedSymbol;
};

// Used only internally
class UndefinedType : public Type {
public:
	UndefinedType(const ast::Node& astNode)
		: Type(Node::UNDEFINED_TYPE, astNode) {
	}

	virtual bool equals(const Type*) const { return false; }	
	virtual std::string name() const { return "<undefined>"; }

	static TypePtr singleton() {
		// Have you ever seen a multithreaded compiler? Huh? HUH?!
		static ast::DummyNode dummy;
		static shared_ptr<UndefinedType> ptr(new UndefinedType(dummy));
		return ptr;
	}
};

typedef shared_ptr<UndefinedType> UndefinedTypePtr;

class IntegralType : public Type {
public:
	IntegralType(const ast::IntegralType& astNode)
		: Type(Node::INTEGRAL_TYPE, astNode),
		  type(astNode.type) {
	}

	IntegralType(const ast::Node& astNode, ast::IntegralType::Kind type)
		: Type(Node::INTEGRAL_TYPE, astNode),
		  type(type) {
	}

	virtual bool equals(const Type* other) const {
		const IntegralType* type = other->isA<IntegralType>();
		if (!type) return false;

		return type->type == this->type;
	}

	virtual std::string name() const {
		switch (type) {
		case ast::IntegralType::I32:
			return "i32";
		case ast::IntegralType::VOID:
			return "void";
		case ast::IntegralType::CHAR:
			return "char";
		case ast::IntegralType::BOOL:
			return "bool";
		default:
			assert(false);
		}
	}

	const ast::IntegralType::Kind type;
};

typedef shared_ptr<IntegralType> IntegralTypePtr;

class FunctionType : public Type {
public:
	typedef std::list<TypePtr> parameter_type_list_t;

	FunctionType(const ast::Node& astNode,
	             TypePtr returnType, 
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

	virtual std::string name() const {
		std::stringstream ss;

		ss << "fn "
		   << returnType->name()
		   << " (";

		size_t i = 0;
		for (auto it = parameterTypes.begin();
		     it != parameterTypes.end();
		     ++it, ++i) {
			ss << (*it)->name();

			if (i < parameterTypes.size())
				ss << ", ";
		}

		ss << ")";

		return ss.str();
	}

	TypePtr returnType;
	parameter_type_list_t parameterTypes;	
};

typedef shared_ptr<FunctionType> FunctionTypePtr;

class ArrayType : public Type {
public:
	TypePtr inner;

	ArrayType(const ast::Node& astNode, TypePtr inner)
		: Type(Node::ARRAY_TYPE, astNode), inner(inner) {
	}

	virtual bool equals(const Type* other) const {
		const ArrayType* type = other->isA<ArrayType>();
		if (!type) return false;

		return type->inner->equals(inner);
	}

	virtual std::string name() const {
		std::stringstream ss;

		ss << inner->name()
		   << "[]";

		return ss.str();
	}
};

typedef shared_ptr<ArrayType> ArrayTypePtr;

} // namespace semantic
} // namespace llang

#endif
