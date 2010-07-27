#ifndef LLANG_AST_NODE_HPP_INCLUDED
#define LLANG_AST_NODE_HPP_INCLUDED

#include "common/location.hpp"

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

private:
	const Location location_;
};

} // namespace ast
} // namespace llang

#endif
