#ifndef LLANG_AST_TYPE_HPP_INCLUDED
#define LLANG_AST_TYPE_HPP_INCLUDED

#include "ast/node.hpp"

namespace llang {
namespace ast {

class Type : public Node {
protected:
	Type(const Location& location)
		: Node(location) {
	}
};

class VoidType : public Type {
public:
	VoidType(const Location& location)
		: Type(location) {
	}
};

class IntType : public Type {
public:
	IntType(const Location& location)
		: Type(location) {
	}
};

} // namespace ast
} // namespace llang

#endif
