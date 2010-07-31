#ifndef LLANG_AST_NODE_HPP_INCLUDED
#define LLANG_AST_NODE_HPP_INCLUDED

#include <cassert>

#include "common/location.hpp"
#include "ast/node_table.hpp"

namespace llang {
namespace ast {

class Node {
public:
	// Create an enum of Node types
#define GENERATE_ENUM_ENTRY(name, nameInCaps) nameInCaps,
	enum Tag {
		LLANG_AST_NODE_TABLE(GENERATE_ENUM_ENTRY)
		TYPE_MAX
	};
#undef GENERATE_ENUM_ENTRY

	Node(const Tag tag, const Location& location)
		: tag(tag), location_(location) {
	}

	virtual ~Node() {}
	
	virtual Location location() const {
		return location_;
	}

	const Tag tag;

private:
	const Location location_;
};

class DummyNode : public Node {
public:	
	DummyNode()
		: Node(Node::DUMMY_NODE, Location("", 0, 0)) {
	}

	virtual Location location() const {
		assert(false);
		return Node::location();
	}
};

} // namespace ast
} // namespace llang

#endif
