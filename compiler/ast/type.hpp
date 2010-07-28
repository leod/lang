#ifndef LLANG_AST_TYPE_HPP_INCLUDED
#define LLANG_AST_TYPE_HPP_INCLUDED

#include "ast/node.hpp"

namespace llang {
namespace ast {

class Type : public Node {
protected:
	Type(const Node::Type type, const Location& location)
		: Node(type, location) {
	}
};

class VoidType : public Type {
public:
	VoidType(const Location& location)
		: Type(Node::VOID_TYPE, location) {
	}
};

class IntType : public Type {
public:
	IntType(const Location& location)
		: Type(Node::INT_TYPE, location) {
	}
};

} // namespace ast
} // namespace llang

#endif
