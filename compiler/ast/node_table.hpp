#ifndef LLANG_AST_NODE_TABLE_HPP_INCLUDED
#define LLANG_AST_NODE_TABLE_HPP_INCLUDED

// see http://www.drdobbs.com/blog/archives/2010/06/the_x_macro.html

#define LLANG_AST_NODE_TABLE(X) \
	X(Node, NODE) \
	X(Type, TYPE) \
	X(Declaration, DECLARATION) \
	X(Module, MODULE) \
	X(FunctionDeclaration, FUNCTION_DECLARATION) \
	X(VariableDeclaration, VARIABLE_DECLARATION) \
	\
	X(VoidType, VOID_TYPE) \
	X(IntType, INT_TYPE) \
    \
	X(Expression, EXPRESSION) \
	X(LiteralNumberExpression, LITERAL_NUMBER_EXPRESSION) \
	X(BlockExpression, BLOCK_EXPRESSION) \
	X(IfElseExpression, IF_ELSE_EXPRESSION) \
	X(VoidExpression, VOID_EXPRESSION)

#endif
