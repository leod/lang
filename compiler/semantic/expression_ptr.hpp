#ifndef LLANG_SEMANTIC_EXPRESSION_PTR_HPP_INCLUDED
#define LLANG_SEMANTIC_EXPRESSION_PTR_HPP_INCLUDED

#include "util/smart_ptr.hpp"

namespace llang {
namespace semantic {

class Expression;
typedef shared_ptr<Expression> ExpressionPtr;

} // namespace semantic
} // namespace llang

#endif
