#ifndef LLANG_AST_NODE_HPP_INCLUDED
#define LLANG_AST_NODE_HPP_INCLUDED

#include "common/location.hpp"
#include "ast/node_table.hpp"

namespace llang {
namespace ast {

class Node {
public:
	explicit Node(const Location& location)
		: location_(location) {
	}

	virtual ~Node() {}
	
	template<typename T> T* is() {
		return dynamic_cast<T*>(this);
	}

	const Location& location() const { return location_; }

	// Create an enum of Node types
#define TABLE_CALLBACK(name, nameInCaps) nameInCaps,
	enum Type {
		LLANG_AST_NODE_TABLE(TABLE_CALLBACK)
	};
#undef TABLE_CALLBACK

private:
	const Location location_;
};

// Looks up the Node::Type corresponding to a type
template<typename T> struct GetNodeType {

};

#define TABLE_CALLBACK(name, nameInCaps) \
	template <>                          \
	struct GetNodeType <name*> {         \
		Node::Type type =                \
			Node::nameInCaps;            \
	};                                   
LLANG_AST_NODE_TABLE(TABLE_CALLBACK)
#undef TABLE_CALLBACK

} // namespace ast
} // namespace llang

#endif
