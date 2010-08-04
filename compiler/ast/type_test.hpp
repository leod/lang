#ifndef LLANG_AST_TYPE_TEST_HPP_INCLUDED
#define LLANG_AST_TYPE_TEST_HPP_INCLUDED

#include "ast/type.hpp"

namespace llang {
namespace ast {

inline bool isBool(TypePtr type) {
	IntegralTypePtr	integral = isA<IntegralType>(type);
	return integral && integral->type == ast::IntegralType::BOOL;
}

inline bool isI32(TypePtr type) {
	IntegralTypePtr	integral = isA<IntegralType>(type);
	return integral && integral->type == ast::IntegralType::I32;
}

inline bool isChar(TypePtr type) {
	IntegralTypePtr	integral = isA<IntegralType>(type);
	return integral && integral->type == ast::IntegralType::CHAR;
}

inline bool isVoid(TypePtr type) {
	IntegralTypePtr	integral = isA<IntegralType>(type);
	return integral && integral->type == ast::IntegralType::VOID;
}

inline bool isArray(TypePtr type) {
	return isA<ArrayType>(type);
}

} // namespace ast
} // namespace llang

#endif
