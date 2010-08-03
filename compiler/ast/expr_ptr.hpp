#ifndef LLANG_AST_EXPR_PTR_HPP_INCLUDED
#define LLANG_AST_EXPR_PTR_HPP_INCLUDED

#include "util/smart_ptr.hpp"

namespace llang {
namespace ast {

class Expr;
typedef shared_ptr<Expr> ExprPtr;

} // namespace ast
} // namespace llang

#endif
