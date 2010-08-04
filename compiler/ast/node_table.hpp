#ifndef LLANG_AST_NODE_TABLE_HPP_INCLUDED
#define LLANG_AST_NODE_TABLE_HPP_INCLUDED

// see http://www.drdobbs.com/blog/archives/2010/06/the_x_macro.html

#define LLANG_AST_NODE_TABLE(X) \
	X(Module, MODULE) \
	X(FunctionDecl, FUNCTION_DECL) \
	X(VariableDecl, VARIABLE_DECL) \
	X(ParameterDecl, PARAMETER_DECL) \
	X(DelayedDecl, DELAYED_DECL) \
	\
	X(IntegralType, INTEGRAL_TYPE) \
	X(UndefinedType, UNDEFINED_TYPE) \
	X(DelayedType, DELAYED_TYPE) \
	X(FunctionType, FUNCTION_TYPE) \
	X(ArrayType, ARRAY_TYPE) \
	\
	X(BinaryExpr, BINARY_EXPR) \
	X(LiteralNumberExpr, LITERAL_NUMBER_EXPR) \
	X(LiteralStringExpr, LITERAL_STRING_EXPR) \
	X(LiteralBoolExpr, LITERAL_BOOL_EXPR) \
	X(BlockExpr, BLOCK_EXPR) \
	X(IfElseExpr, IF_ELSE_EXPR) \
	X(VoidExpr, VOID_EXPR) \
	X(IdentifierExpr, IDENTIFIER_EXPR) \
	X(CallExpr, CALL_EXPR) \
	X(DeclRefExpr, DECL_REF_EXPR) \
	X(DeclExpr, DECL_EXPR) \
	X(DelayedExpr, DELAYED_EXPR) \
	X(ArrayElementExpr, ARRAY_ELEMENT_EXPR)

#endif
