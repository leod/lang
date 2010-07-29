#ifndef LLANG_SEMANTIC_TYPE_HPP_INCLUDED
#define LLANG_SEMANTIC_TYPE_HPP_INCLUDED

#include "semantic/node.hpp"

namespace llang {
namespace semantic {

class Type : public Node {
protected:
	Type(const Node::Tag tag, const ast::Node& astNode)
		: Node(tag, astNode) {
	}
};

class IntegralType : public Type {
public:
	IntegralType(const ast::Node& astNode)
		: Type(Node::INTEGRAL_TYPE, astNode) {
	}
};

} // namespace ast
} // namespace llang

#endif
