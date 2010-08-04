#ifndef LLANG_AST_TYPE_HPP_INCLUDED
#define LLANG_AST_TYPE_HPP_INCLUDED

#include <list>
#include <sstream>
#include <string>

#include "ast/node.hpp"
#include "ast/decl_ptr.hpp"
#include "ast/type_ptr.hpp"

namespace llang {
namespace ast {

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
	virtual bool canCastImplicitly(const TypePtr) const;

protected:
	Type(const Node::Tag tag, const Location& location)
		: Node(tag, location) {
	}
};

// Used internally
class DelayedType : public Type {
public:
	DelayedType(const Location& location, DeclPtr delayedDecl)
		: Type(Node::DELAYED_TYPE, location),
		  delayedDecl(delayedDecl) {
	}

	virtual bool equals(const Type*) const { return false; }	
	virtual std::string name() const { return "<undefined>"; }

	DeclPtr delayedDecl;
};

// Used internally
class UndefinedType : public Type {
public:
	UndefinedType(const Location& location)
		: Type(Node::UNDEFINED_TYPE, location) {
	}

	virtual bool equals(const Type*) const { return false; }	
	virtual std::string name() const { return "<undefined>"; }

	static TypePtr singleton() {
		// Have you ever seen a multithreaded compiler? Huh? HUH?!
		static Location location("", 0, 0);
		static shared_ptr<UndefinedType> ptr(new UndefinedType(location));
		return ptr;
	}
};

typedef shared_ptr<UndefinedType> UndefinedTypePtr;

class IntegralType : public Type {
public:
	enum Kind {
		I32,
		CHAR,
		BOOL,
		VOID,
		STRING
	};

	IntegralType(const Location& location, Kind type)
		: Type(Node::INTEGRAL_TYPE, location),
		  type(type) {
	}

	virtual bool equals(const Type* other) const {
		const IntegralType* type = other->isA<IntegralType>();
		if (!type) return false;

		return type->type == this->type;
	}

	virtual std::string name() const {
		switch (type) {
		case IntegralType::I32:
			return "i32";
		case IntegralType::VOID:
			return "void";
		case IntegralType::CHAR:
			return "char";
		case IntegralType::BOOL:
			return "bool";
		default:
			assert(false);
		}
	}

	const IntegralType::Kind type;
};

typedef shared_ptr<IntegralType> IntegralTypePtr;

// Used internally
class NumberType : public Type {
public:
	NumberType(const Location& location)
		: Type(Node::NUMBER_TYPE, location) {
	}

	virtual bool equals(const Type* other) const {
		const NumberType* type = other->isA<NumberType>();
		return type;
	}

	virtual std::string name() const {
		return "<number>";
	}

	virtual bool canCastImplicitly(const TypePtr) const;
};

class FunctionType : public Type {
public:
	typedef std::list<TypePtr> ParameterTypeList;

	FunctionType(const Location& location,
	             TypePtr returnType, 
	             ParameterTypeList& parameterTypes)
		: Type(Node::FUNCTION_TYPE, location),
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
	ParameterTypeList parameterTypes;	
};

typedef shared_ptr<FunctionType> FunctionTypePtr;

class ArrayType : public Type {
public:
	TypePtr inner;

	ArrayType(const Location& location, TypePtr inner)
		: Type(Node::ARRAY_TYPE, location), inner(inner) {
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

} // namespace ast
} // namespace llang

#endif
