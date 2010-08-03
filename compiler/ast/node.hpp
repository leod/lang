#ifndef LLANG_AST_NODE_HPP_INCLUDED
#define LLANG_AST_NODE_HPP_INCLUDED

#include "util/smart_ptr.hpp"
#include "common/location.hpp"
#include "ast/node_table.hpp"

namespace llang {
namespace ast {

class Node {
public:
#define GENERATE_ENUM_ENTRY(name, nameInCaps) nameInCaps,
	enum Tag {
		LLANG_AST_NODE_TABLE(GENERATE_ENUM_ENTRY)
		TYPE_MAX
	};
#undef GENERATE_ENUM_ENTRY

	Node(Tag tag, const Location& location)
		: tag(tag), location_(location) {
	}

	virtual ~Node() {}

	template <typename T> T* isA() {
		return dynamic_cast<T*>(this);
	}

	template <typename T> const T* isA() const {
		return dynamic_cast<const T*>(this);
	}

	const Tag tag;

	Location location() const {
		return location_;
	}

private:
	const Location location_;
};

typedef shared_ptr<Node> NodePtr;

template <typename T, typename U> shared_ptr<T> isA(shared_ptr<U> p) {
	return dynamic_pointer_cast<T>(p);
}

template <typename T, typename U> shared_ptr<T> assumeIsA(shared_ptr<U> p) {
	shared_ptr<T> t = isA<T>(p);
	assert(t.get());

	return t;
}

} // namespace semantic
} // namespace llang

#endif
