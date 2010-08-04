#ifndef LLANG_PARSER_PARSER_HPP_INCLUDED
#define LLANG_PARSER_PARSER_HPP_INCLUDED

#include "common/context.hpp"
#include "lexer/token_stream.hpp"

namespace llang {

namespace ast {

class Type;
class Decl;
class Module;
class Expr;

} // namespace ast

namespace parser {

class Parser {
public:
	Parser(Context& context,
	       const std::string& moduleName,
	       lexer::TokenStream& ts)
		: diag(context.diag), moduleName(moduleName), ts(ts) {
	}

	ast::ModulePtr parseModule();

private:
	ast::TypePtr parseType();
	ast::DeclPtr parseDecl();
	ast::ExprPtr parseExpr();

	ast::TypePtr parseFunctionType();

	ast::DeclPtr parseFunctionDecl();
	ast::DeclPtr parseVariableDecl();

	ast::ExprPtr parseBlockExpr();
	ast::ExprPtr parseIfElseExpr();

	ast::ExprPtr parseAssignExpr();
	ast::ExprPtr parseEqualsExpr();
	ast::ExprPtr parseAddExpr();
	ast::ExprPtr parseMulExpr();
	ast::ExprPtr parsePrimaryExpr();
	ast::ExprPtr parsePostExpr(ast::ExprPtr expr);

	void parseFunctionPrototype(ast::TypePtr& returnType, identifier_t& name,
	                            ast::FunctionDecl::ParameterList& parameters);
	identifier_t parseIdentifier();

	// Checks if current token is a 'type'
	void assume(lexer::Token::Type type);

	// Checks if current token is a 'type' and moves to next token
	void assumeNext(lexer::Token::Type type);

	// Moves to next token and checks if it is a 'type.
	// Returns the token before moving.
	const lexer::Token& expectNext(lexer::Token::Type type);

	void error(const char* format, ...);
	void expectedError(const char* expected);

	Diagnostics& diag;

	const std::string& moduleName;
	lexer::TokenStream& ts;
};

} // namespace parser
} // namespace llang

#endif
