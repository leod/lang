#ifndef LLANG_SEMANTIC_TYPE_TEST_HPP_INCLUDED
#define LLANG_SEMANTIC_TYPE_TEST_HPP_INCLUDED

#include "semantic/type.hpp"

namespace llang {
namespace semantic {

bool isBool(TypePtr type) {
	IntegralTypePtr	integral = isA<IntegralType>(type);
	return integral && integral->type == ast::IntegralType::BOOL;
}

} // namespace semantic
} // namespace llang

#endif
