#ifndef LLANG_SEMANTIC_NODE_TABLE_HPP_INCLUDED
#define LLANG_SEMANTIC_NODE_TABLE_HPP_INCLUDED

#define LLANG_SEMANTIC_NODE_TABLE(X) \
	X(Module, MODULE) \
	X(FunctionSymbol, FUNCTION_SYMBOL) \
	X(VariableSymbol, VARIABLE_SYMBOL) \
	\
	X(IntegralType, INTEGRAL_TYPE) \
	\
	X(BinaryExpression, BINARY_EXPRESSION) \
	X(LiteralNumberExpression, LITERAL_NUMBER_EXPRESSION) \
	X(BlockExpression, BLOCK_EXPRESSION) \
	X(IfElseExpression, IF_ELSE_EXPRESSION) \
	X(VoidExpression, VOID_EXPRESSION) \
	X(IdentifierExpression, IDENTIFIER_EXPRESSION) \
	X(CallExpression, CALL_EXPRESSION) \
	X(DeclarationExpression, DECLARATION_EXPRESSION)

#endif
