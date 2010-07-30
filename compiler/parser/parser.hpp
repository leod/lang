#ifndef LLANG_PARSER_PARSER_HPP_INCLUDED
#define LLANG_PARSER_PARSER_HPP_INCLUDED

#include "common/context.hpp"
#include "lexer/token_stream.hpp"
#include "ast/declaration.hpp"
#include "ast/type.hpp"
#include "ast/expression.hpp"

namespace llang {
namespace parser {

class Parser {
public:
	Parser(Context& context,
	       const std::string& moduleName,
	       lexer::TokenStream& ts)
		: diag(context.diag), moduleName(moduleName), ts(ts) {
	}

	ast::Module* parseModule();

private:
	ast::Declaration* parseDeclaration();
	ast::Type* parseType();
	ast::Expression* parseExpression();

	ast::Declaration* parseFunctionDeclaration();
	ast::Declaration* parseVariableDeclaration();

	ast::Expression* parseBlockExpression();
	ast::Expression* parseIfElseExpression();

	ast::Expression* parseAssignExpression();
	ast::Expression* parseEqualsExpression();
	ast::Expression* parseAddExpression();
	ast::Expression* parseMulExpression();
	ast::Expression* parsePrimaryExpression();
	ast::Expression* parsePostExpression(ast::Expression* expression);

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
