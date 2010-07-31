#ifndef LLANG_SEMANTIC_VISITOR_HPP_INCLUDED
#define LLANG_SEMANTIC_VISITOR_HPP_INCLUDED

#include "semantic/node.hpp"
#include "semantic/node_table.hpp"

namespace llang {
namespace semantic {

#define LLANG_VISITOR_TABLE_PARAM LLANG_SEMANTIC_NODE_TABLE
#define LLANG_VISITOR_TYPE_PARAM  Node
#define LLANG_VISITOR_TAG_PARAM   tag

#include "util/make_visitor.hpp"

#undef LLANG_VISITOR_TABLE_PARAM
#undef LLANG_VISITOR_TYPE_PARAM
#undef LLANG_VISITOR_TAG_PARAM

} // namespace ast
} // namespace llang

#endif
