#include "ast/type.hpp"
#include "ast/type_test.hpp"

namespace llang {
namespace ast {

bool Type::canCastImplicitly(const TypePtr other) const {
	return isVoid(other);
}

bool NumberType::canCastImplicitly(const TypePtr other) const {
	return Type::canCastImplicitly(other) || isI32(other) || isChar(other);
}

bool IntegralType::canCastImplicitly(const TypePtr other) const {
	return Type::canCastImplicitly(other) ||
		(type == I32 && isChar(other));	
}

} // namespace ast
} // namespace llang
