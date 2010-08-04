#ifndef LLANG_AST_TYPE_TEST_HPP_INCLUDED
#define LLANG_AST_TYPE_TEST_HPP_INCLUDED

#include "ast/type.hpp"

namespace llang {
namespace ast {

bool isBool(TypePtr type) {
	IntegralTypePtr	integral = isA<IntegralType>(type);
	return integral && integral->type == ast::IntegralType::BOOL;
}

bool isI32(TypePtr type) {
	IntegralTypePtr	integral = isA<IntegralType>(type);
	return integral && integral->type == ast::IntegralType::I32;
}

bool isArray(TypePtr type) {
	return isA<ArrayType>(type);
}

} // namespace ast
} // namespace llang

#endif
