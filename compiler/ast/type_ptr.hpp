#ifndef LLANG_AST_TYPE_PTR_HPP_INCLUDED
#define LLANG_AST_TYPE_PTR_HPP_INCLUDED

#include "util/smart_ptr.hpp"

namespace llang {
namespace ast {

class Type;
typedef shared_ptr<Type> TypePtr;

} // namespace ast
} // namespace llang

#endif
