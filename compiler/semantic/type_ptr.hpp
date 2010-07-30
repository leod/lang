#ifndef LLANG_SEMANTIC_TYPE_PTR_HPP_INCLUDED
#define LLANG_SEMANTIC_TYPE_PTR_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

namespace llang {
namespace semantic {

class Type;
typedef boost::shared_ptr<Type> TypePtr;

} // namespace semantic
} // namespace llang

#endif
