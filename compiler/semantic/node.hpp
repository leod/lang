#ifndef LLANG_SEMANTIC_NODE_HPP_INCLUDED
#define LLANG_SEMANTIC_NODE_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

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

	virtual ~Node() {}

	template <typename T> T* isA() {
		return dynamic_cast<T*>(this);
	}

	template <typename T> const T* isA() const {
		return dynamic_cast<const T*>(this);
	}

	const Tag tag;
	const ast::Node& astNode;
};

template <typename T, typename U>
boost::shared_ptr<T> isA(boost::shared_ptr<U> p) {
	// TODO: use Tag to check type
	return boost::dynamic_pointer_cast<T>(p);
}

} // namespace semantic
} // namespace llang

#endif
