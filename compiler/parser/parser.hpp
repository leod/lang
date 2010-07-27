#ifndef LLANG_PARSER_PARSER_HPP_INCLUDED
#define LLANG_PARSER_PARSER_HPP_INCLUDED

#include "lexer/token_stream.hpp"
#include "ast/declaration.hpp"
#include "ast/type.hpp"
#include "ast/expression.hpp"

namespace llang {
namespace parser {

class Parser {
public:
	Parser(const std::string& moduleName, lexer::TokenStream& ts)
		: moduleName(moduleName), ts(ts) {
	}

	ast::Module* parseModule();

private:
	ast::Declaration* parseDeclaration();
	ast::Type* parseType();
	ast::Expression* parseExpression();

	const std::string& moduleName;
	lexer::TokenStream& ts;
};

} // namespace parser
} // namespace llang

#endif
