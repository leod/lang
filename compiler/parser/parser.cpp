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
	const Location location = ts.get().location;

	switch (ts.get().type) {
	case Token::KEYWORD_I32:
		return new I32Type(location);

	case Token::KEYWORD_VOID:
		return new VoidType(location);

	default:
		expectedError("type");
		assert(false);
	}
}

Expression* Parser::parseExpression() {
	assert(false);
}

Declaration* Parser::parseFunctionDeclaration() {
	assert(false);
}

Declaration* Parser::parseVariableDeclaration() {
	const Location location = ts.get().location;

	assumeCurrent(Token::KEYWORD_VAR);
	ts.next();
	
	// TODO: parse inferred declarations
	const Type* type = parseType();

	const identifier_t identifier = parseIdentifier();

	if (ts.get().type == Token::SEMICOLON) {
		assert(false); // TODO: implement vars without initializer
	} else {
		nextExpect(Token::EQUALS);
		const Expression* initializer = parseExpression();

		return new VariableDeclaration(location, type, identifier, initializer);
	}
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

identifier_t Parser::parseIdentifier() {
	assumeCurrent(Token::IDENTIFIER);
	const identifier_t identifier = ts.get().identifier;
	ts.next();

	return identifier;
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
