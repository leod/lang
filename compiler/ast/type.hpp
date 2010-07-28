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

class VoidType : public Type {
public:
	VoidType(const Location& location)
		: Type(Node::VOID_TYPE, location) {
	}
};

class I32Type : public Type {
public:
	I32Type(const Location& location)
		: Type(Node::I32_TYPE, location) {
	}
};

} // namespace ast
} // namespace llang

#endif
