#ifndef LLANG_AST_NODE_HPP_INCLUDED
#define LLANG_AST_NODE_HPP_INCLUDED

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
		: tag(tag), location(location) {
	}

	virtual ~Node() {}
	
	const Tag tag;
	const Location location;
};

} // namespace ast
} // namespace llang

#endif
