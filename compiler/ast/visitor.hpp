#ifndef LLANG_AST_VISITOR_HPP_INCLUDED
#define LLANG_AST_VISITOR_HPP_INCLUDED

#include "ast/node.hpp"
#include "ast/node_table.hpp"

namespace llang {
namespace ast {

#define LLANG_VISITOR_TABLE_PARAM LLANG_AST_NODE_TABLE
#define LLANG_VISITOR_TYPE_PARAM  Node
#define LLANG_VISITOR_TAG_PARAM   tag

#include "common/make_visitor.hpp"

#undef LLANG_VISITOR_TABLE_PARAM
#undef LLANG_VISITOR_TYPE_PARAM
#undef LLANG_VISITOR_TAG_PARAM

} // namespace ast
} // namespace llang

#endif
