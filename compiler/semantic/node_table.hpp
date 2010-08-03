#ifndef LLANG_SEMANTIC_NODE_TABLE_HPP_INCLUDED
#define LLANG_SEMANTIC_NODE_TABLE_HPP_INCLUDED

#define LLANG_SEMANTIC_NODE_TABLE(X) \
	X(DelayedSymbol, DELAYED_SYMBOL) \
	X(Module, MODULE) \
	X(FunctionSymbol, FUNCTION_SYMBOL) \
	X(VariableSymbol, VARIABLE_SYMBOL) \
	X(ParameterSymbol, PARAMETER_SYMBOL) \
	\
	X(DelayedType, DELAYED_TYPE) \
	X(UndefinedType, UNDEFINED_TYPE) \
	X(IntegralType, INTEGRAL_TYPE) \
	X(FunctionType, FUNCTION_TYPE) \
	X(ArrayType, ARRAY_TYPE) \
	\
	X(DelayedExpression, DELAYED_EXPRESSION) \
	X(BinaryExpression, BINARY_EXPRESSION) \
	X(LiteralNumberExpression, LITERAL_NUMBER_EXPRESSION) \
	X(LiteralStringExpression, LITERAL_STRING_EXPRESSION) \
	X(BlockExpression, BLOCK_EXPRESSION) \
	X(IfElseExpression, IF_ELSE_EXPRESSION) \
	X(VoidExpression, VOID_EXPRESSION) \
	X(CallExpression, CALL_EXPRESSION) \
	X(SymbolExpression, SYMBOL_EXPRESSION) \
	X(DeclarationExpression, DECLARATION_EXPRESSION)

#endif
