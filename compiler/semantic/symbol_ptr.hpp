#ifndef LLANG_SEMANTIC_SYMBOL_PTR_HPP_INCLUDED
#define LLANG_SEMANTIC_SYMBOL_PTR_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

namespace llang {
namespace semantic {

class Symbol;
typedef boost::shared_ptr<Symbol> SymbolPtr;

} // namespace semantic
} // namespace llang

#endif
