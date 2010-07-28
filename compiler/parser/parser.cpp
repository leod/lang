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
	Location location = ts.get().location;

	switch (ts.get().type) {
	case Token::KEYWORD_I32:
		ts.next();
		return new I32Type(location);

	case Token::KEYWORD_VOID:
		ts.next();
		return new VoidType(location);

	default:
		expectedError("type");
		assert(false);
	}
}

Expression* Parser::parseExpression() {
	return parseLiteralExpression();
}

Declaration* Parser::parseFunctionDeclaration() {
	const Location location = ts.get().location;

	assumeNext(Token::KEYWORD_FN);
	
	Type* returnType = parseType();	
	identifier_t identifier = parseIdentifier();

	assumeNext(Token::LPAREN);

	FunctionDeclaration::parameter_list_t parameters;

	do {
		Type* type = parseType();

		bool hasName = false;	
		identifier_t name;

		if (ts.get().type == Token::IDENTIFIER) {
			hasName = true;
			name = parseIdentifier();
		}

		parameters.push_back(FunctionDeclaration::Parameter(
			type, hasName, name));	
	} while(ts.get().type == Token::COMMA);

	assumeNext(Token::RPAREN);
	assumeNext(Token::EQUALS);

	Expression* body = parseExpression();

	return new FunctionDeclaration(location, returnType, identifier, parameters,
	                               body);
}

Declaration* Parser::parseVariableDeclaration() {
	const Location location = ts.get().location;

	assumeNext(Token::KEYWORD_VAR);
	
	// TODO: parse inferred declarations
	Type* type = parseType();

	identifier_t identifier = parseIdentifier();

	if (ts.get().type == Token::SEMICOLON) {
		assert(false); // TODO: implement vars without initializer
	} else {
		assumeNext(Token::EQUALS);

		const Expression* initializer = parseExpression();

		return new VariableDeclaration(location, type, identifier, initializer);
	}
}

Expression* Parser::parseBlockExpression() {
	const Location location = ts.get().location;

	assumeNext(Token::LBRACE);

	BlockExpression::expression_list_t expressions;

	while (ts.get().type != Token::RBRACE) {
		expressions.push_back(ExpressionPtr(parseExpression()));
	}

	assumeNext(Token::RBRACE);

	return new BlockExpression(location, expressions);
}

Expression* Parser::parseLiteralExpression() {
	const Location location = ts.get().location;

	switch (ts.get().type) {
	case Token::NUMBER:
		ts.next();
		return new LiteralNumberExpression(location, ts.get().number);

	case Token::KEYWORD_VOID:
		ts.next(); 
		return new VoidExpression(location);
			
	default:
		expectedError("literal");	
		assert(false);
	}
}

Expression* Parser::parseIfElseExpression() {
	const Location location = ts.get().location;

	assumeNext(Token::KEYWORD_IF);

	assumeNext(Token::LPAREN);
	Expression* condition = parseExpression();
	assumeNext(Token::RPAREN);

	Expression* ifBody = parseExpression();

	assumeNext(Token::KEYWORD_ELSE);
	Expression* elseBody = parseExpression();

	return new IfElseExpression(location, condition, ifBody, elseBody);
}

void Parser::assume(lexer::Token::Type type) {
	const Token& token = ts.get();
	if (token.type != type) {
		error("expected %s, not %s",
			Token::typeToString(type), Token::typeToString(token.type));
	}
}

void Parser::assumeNext(lexer::Token::Type type) {
	assume(type);
	ts.next();
}

const Token& Parser::expectNext(lexer::Token::Type type) {
	const Token& token = ts.next();
	assumeNext(type);

	return token;
}

identifier_t Parser::parseIdentifier() {
	assume(Token::IDENTIFIER);
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
