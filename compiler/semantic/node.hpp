#ifndef LLANG_SEMANTIC_NODE_HPP_INCLUDED
#define LLANG_SEMANTIC_NODE_HPP_INCLUDED

#include "ast/node.hpp"
#include "semantic/node_table.hpp"

namespace llang {
namespace semantic {

class Node {
public:
#define GENERATE_ENUM_ENTRY(name, nameInCaps) nameInCaps,
	enum Tag {
		LLANG_SEMANTIC_NODE_TABLE(GENERATE_ENUM_ENTRY)
		TYPE_MAX
	};
#undef GENERATE_ENUM_ENTRY

	Node(Tag tag, const ast::Node& astNode)
		: tag(tag), astNode(astNode) {
	}

	const Tag tag;
	const ast::Node& astNode;
};

} // namespace semantic
} // namespace llang

#endif
