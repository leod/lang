#include "ast/type.hpp"
#include "ast/expression.hpp"
#include "ast/declaration.hpp"

namespace llang {
namespace ast {

FunctionDeclaration::Parameter::Parameter(Type* type, 
                                          bool hasName,
                                          const identifier_t& name)
	: type(type), hasName(hasName), name(name) {}

FunctionDeclaration::Parameter::~Parameter() {}

FunctionDeclaration::FunctionDeclaration(const Location& location,
                                         Type* returnType,
                                         const identifier_t& name,
                                         parameter_list_t& parameters,
                                         Expression* body)
	: Declaration(Node::FUNCTION_DECLARATION, location),
	  returnType(returnType),
	  name(name),
	  parameters(parameters),
	  body(body) {}

FunctionDeclaration::~FunctionDeclaration() {}

VariableDeclaration::VariableDeclaration(const Location& location,
                                         Type* type,
                                         const identifier_t& name,
                                         Expression* initializer)
	: Declaration(Node::VARIABLE_DECLARATION, location),
	  type(type),
	  name(name),
	  initializer(initializer) {}

VariableDeclaration::~VariableDeclaration() {}

} // namespace ast
} // namespace llang
