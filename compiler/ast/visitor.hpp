#ifndef LLANG_AST_VISITOR_HPP_INCLUDED
#define LLANG_AST_VISITOR_HPP_INCLUDED

#include "ast/node.hpp"
#include "ast/node_table.hpp"

namespace llang {
namespace ast {

#define LLANG_VISITOR_TABLE_PARAM           LLANG_AST_NODE_TABLE
#define LLANG_VISITOR_TYPE_PARAM            Node
#define LLANG_VISITOR_TAG_PARAM             tag
#define LLANG_VISITOR_TYPE_WRAP_PARAM(type) type&
#define LLANG_VISITOR_CAST_PARAM(type)      static_cast<type&>
#define LLANG_VISITOR_MEMBER_PARAM          .

#include "util/make_visitor.hpp"

#undef LLANG_VISITOR_TABLE_PARAM
#undef LLANG_VISITOR_TYPE_PARAM
#undef LLANG_VISITOR_TAG_PARAM
#undef LLANG_VISITOR_TYPE_WRAP_PARAM
#undef LLANG_VISITOR_CAST_PARAM
#undef LLANG_VISITOR_MEMBER_PARAM

} // namespace ast
} // namespace llang

#endif
