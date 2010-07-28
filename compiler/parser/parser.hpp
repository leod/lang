#ifndef LLANG_PARSER_PARSER_HPP_INCLUDED
#define LLANG_PARSER_PARSER_HPP_INCLUDED

#include "common/diagnostics.hpp"
#include "lexer/token_stream.hpp"
#include "ast/declaration.hpp"
#include "ast/type.hpp"
#include "ast/expression.hpp"

namespace llang {
namespace parser {

class Parser {
public:
	Parser(Diagnostics& diag,
	       const std::string& moduleName,
	       lexer::TokenStream& ts)
		: diag(diag), moduleName(moduleName), ts(ts) {
	}

	ast::Module* parseModule();

private:
	ast::Declaration* parseDeclaration();
	ast::Type* parseType();
	ast::Expression* parseExpression();

	ast::Declaration* parseFunctionDeclaration();
	ast::Declaration* parseVariableDeclaration();

	void error(const char* format, ...);
	void expectedError(const char* expected);

	Diagnostics& diag;

	const std::string& moduleName;
	lexer::TokenStream& ts;
};

} // namespace parser
} // namespace llang

#endif
