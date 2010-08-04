#ifndef LLANG_AST_TYPE_TEST_HPP_INCLUDED
#define LLANG_AST_TYPE_TEST_HPP_INCLUDED

#include "ast/type.hpp"

namespace llang {
namespace ast {

inline bool isIntegral(TypePtr type, ast::IntegralType::Kind kind) {
	IntegralTypePtr	integral = isA<IntegralType>(type);
	return integral && integral->type == kind;
}

inline bool isIntegral(TypePtr type) {
	return isA<IntegralType>(type);
}

inline bool isBool(TypePtr type) {
	return isIntegral(type, ast::IntegralType::BOOL);
}

inline bool isI32(TypePtr type) {
	return isIntegral(type, ast::IntegralType::I32);
}

inline bool isChar(TypePtr type) {
	return isIntegral(type, ast::IntegralType::CHAR);
}

inline bool isVoid(TypePtr type) {
	return isIntegral(type, ast::IntegralType::VOID);
}

inline bool isArray(TypePtr type) {
	return isA<ArrayType>(type);
}

} // namespace ast
} // namespace llang

#endif
