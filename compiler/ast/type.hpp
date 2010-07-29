#ifndef LLANG_AST_TYPE_HPP_INCLUDED
#define LLANG_AST_TYPE_HPP_INCLUDED

#include "lexer/token.hpp"
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
	IntegralType(const Location& location, const lexer::Token::Type type)
		: Type(Node::INTEGRAL_TYPE, location), type(type) {
	}

	const lexer::Token::Type type;
};

} // namespace ast
} // namespace llang

#endif
