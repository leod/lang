#ifndef LLANG_SEMANTIC_SYMBOL_PTR_HPP_INCLUDED
#define LLANG_SEMANTIC_SYMBOL_PTR_HPP_INCLUDED

#include "util/smart_ptr.hpp"

namespace llang {
namespace semantic {

class Symbol;
typedef shared_ptr<Symbol> SymbolPtr;

} // namespace semantic
} // namespace llang

#endif
