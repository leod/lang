#ifndef LLANG_AST_VISITOR_HPP_INCLUDED
#define LLANG_AST_VISITOR_HPP_INCLUDED

#include "ast/node.hpp"
#include "ast/node_table.hpp"

namespace llang {
namespace ast {

// Extend node table by abstract classes
#define EXTENDED_NODE_TABLE(X) \
	LLANG_AST_NODE_TABLE(X) \
	X(Node, ) \
	X(Type, ) \
	X(Declaration, ) \
	X(Expression, )

// Generate forward references
#define GENERATE_FORWARD_REFERENCE(name, nameInCaps) \
	class name;
EXTENDED_NODE_TABLE(GENERATE_FORWARD_REFERENCE)
#undef GENERATE_FORWARD_REFERENCE

#define GENERATE_VIRTUAL_METHOD(name, nameCaps) \
	virtual Result visit(name&) { assert(false); }

#define GENERATE_CASE(name, nameCaps) \
		case Node::nameCaps:          \
			return visit(static_cast<name&>(node)); 

template <typename Param, typename Result> class Visitor {
protected:
	// Generate virtual methods
	EXTENDED_NODE_TABLE(GENERATE_VIRTUAL_METHOD)

public:
	Result accept(Node& node, Param param) {
		switch(node.type()) {

		// Generate cases
		LLANG_AST_NODE_TABLE(GENERATE_CASE)

		default:
			assert(false);
		}
	}
};

template <typename Result> class Visitor <void, Result> {
protected:
	EXTENDED_NODE_TABLE(GENERATE_VIRTUAL_METHOD)

public:
	Result accept(Node& node) {
		switch(node.type()) {

		LLANG_AST_NODE_TABLE(GENERATE_CASE)

		default:
			assert(false);
		}
	}
};

#undef GENERATE_VIRTUAL_METHOD	
#undef GENERATE_CASE

} // namespace ast
} // namespace llang

#endif
