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
	enum Type {
		LLANG_AST_NODE_TABLE(GENERATE_ENUM_ENTRY)
		TYPE_MAX
	};
#undef GENERATE_ENUM_ENTRY

	Node(const Type type, const Location& location)
		: type_(type), location_(location) {
	}

	virtual ~Node() {}
	
	template<typename T> T* is() {
		return dynamic_cast<T*>(this);
	}

	Type type() const { return type_; }
	const Location& location() const { return location_; }

private:
	const Type type_;
	const Location location_;
};

} // namespace ast
} // namespace llang

#endif
