#include <cstdarg> 

#include "parser/parser.hpp"

namespace llang {
namespace parser {

using namespace ast;
using namespace lexer;

Module* Parser::parseModule() {
	Module* module = new Module(Location(moduleName, 1, 1), moduleName);

	while (ts.get().type != Token::END_OF_FILE) {
		module->addDeclaration(parseDeclaration());
	}

	return module;
}

Declaration* Parser::parseDeclaration() {
	switch (ts.get().type) {
	case Token::KEYWORD_FN:
		return parseFunctionDeclaration();

	case Token::KEYWORD_VAR:
		return parseVariableDeclaration();

	default:
		expectedError("declaration");		
	}

	assert(false);
}

Type* Parser::parseType() {
	assert(false);
}

Expression* Parser::parseExpression() {
	assert(false);
}

Declaration* Parser::parseFunctionDeclaration() {
	assert(false);
}

Declaration* Parser::parseVariableDeclaration() {
	assert(false);
}

Expression* Parser::parseIfExpression() {
	assert(false);
}

void Parser::assumeCurrent(lexer::Token::Type type) {
	const Token& token = ts.get();
	if (token.type != type) {
		error("expected %s, not %s",
			Token::typeToString(type), Token::typeToString(token.type));
	}
}

const Token& Parser::nextExpect(lexer::Token::Type type) {
	const Token& token = ts.next();
	assumeCurrent(type);

	return token;
}

void Parser::error(const char* format, ...) {
	va_list argp;
	va_start(argp, format);
	diag.verror(ts.get().location, format, argp);
	va_end(argp);

	assert(false);
}

void Parser::expectedError(const char* expected) {
	error("expected %s, not %s",
		expected, Token::typeToString(ts.get().type));
	assert(false);
}

}
}
