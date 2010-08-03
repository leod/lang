#ifndef LLANG_AST_DECL_PTR_HPP_INCLUDED
#define LLANG_AST_DECL_PTR_HPP_INCLUDED

#include "util/smart_ptr.hpp"

namespace llang {
namespace ast {

class Decl;
typedef shared_ptr<Decl> DeclPtr;

} // namespace ast
} // namespace llang

#endif
