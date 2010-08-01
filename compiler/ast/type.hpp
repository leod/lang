#ifndef LLANG_AST_TYPE_HPP_INCLUDED
#define LLANG_AST_TYPE_HPP_INCLUDED

#include "ast/node.hpp"

namespace llang {
namespace ast {

class Type : public Node {
protected:
	Type(const Node::Tag tag, const Location& location)
		: Node(tag, location) {
	}
};

class IntegralType : public Type {
public:
	enum Kind {
		I32,
		CHAR,
		VOID,
		STRING
	};

	IntegralType(const Location& location, const Kind type)
		: Type(Node::INTEGRAL_TYPE, location), type(type) {
	}

	const Kind type;
};

} // namespace ast
} // namespace llang

#endif
