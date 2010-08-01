#include <cstdarg> 
#include <cstdio>

#include "ast/type.hpp"
#include "ast/declaration.hpp"
#include "ast/expression.hpp"
#include "parser/parser.hpp"

namespace llang {
namespace parser {

using namespace ast;
using namespace lexer;

Module* Parser::parseModule() {
	Module::declaration_list_t declarations;

	while (ts.get().type != Token::END_OF_FILE) {
		declarations.push_back(DeclarationPtr(parseDeclaration()));
	}

	return new Module(Location(moduleName, 1, 1), moduleName, declarations);
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
		return new IntegralType(location, Token::KEYWORD_I32);

	case Token::KEYWORD_VOID:
		ts.next();
		return new IntegralType(location, Token::KEYWORD_VOID);

	default:
		expectedError("type");
		assert(false);
	}
}

Expression* Parser::parseExpression() {
	switch (ts.get().type) {
	default:
		return parseAssignExpression();

	case Token::KEYWORD_FN:
	case Token::KEYWORD_VAR:
		const Location location = ts.get().location;
		return new DeclarationExpression(location, parseDeclaration());
	}
}

Declaration* Parser::parseFunctionDeclaration() {
	const Location location = ts.get().location;

	assumeNext(Token::KEYWORD_FN);
	
	Type* returnType = parseType();	
	identifier_t identifier = parseIdentifier();

	assumeNext(Token::LPAREN);

	FunctionDeclaration::parameter_list_t parameters;

	if (ts.get().type != Token::RPAREN) {
		bool doLoop = true;

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

			if (ts.get().type != Token::COMMA)
				doLoop = false;
			else
				ts.next();
		} while(doLoop);
	}

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
		assert(false); // TODO: implement vars without initializers
	} else {
		assumeNext(Token::EQUALS);

		Expression* initializer = parseExpression();

		return new VariableDeclaration(location, type, identifier, initializer);
	}
}

Expression* Parser::parseBlockExpression() {
	const Location location = ts.get().location;

	assumeNext(Token::LBRACE);

	BlockExpression::expression_list_t expressions;

	while (ts.get().type != Token::RBRACE) {
		expressions.push_back(ExpressionPtr(parseExpression()));
		assumeNext(Token::SEMICOLON);
	}

	assumeNext(Token::RBRACE);

	return new BlockExpression(location, expressions);
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

Expression* Parser::parseAssignExpression() {
	Location location = ts.get().location;

	Expression* expression = parseEqualsExpression();
	
	// TODO

	return expression;
}

Expression* Parser::parseEqualsExpression() {
	Location location = ts.get().location;

	Expression* expression = parseAddExpression();
	
	while (ts.get().type == Token::EQUALS) {
		ts.next();

		expression = new BinaryExpression(location, BinaryExpression::EQUALS,
		                                  expression, parseAddExpression());
	}

	return expression;
}

Expression* Parser::parseAddExpression() {
	Location location = ts.get().location;

	Expression* expression = parseMulExpression();
	
	while (ts.get().type == Token::PLUS || ts.get().type == Token::MINUS) {
		BinaryExpression::Operation operation;

		switch (ts.get().type) {
		case Token::PLUS:
			operation = BinaryExpression::ADD;
			break;
		case Token::MINUS:
			operation = BinaryExpression::SUB;
			break;
		default:
			assert(false);
		}

		ts.next();

		expression = new BinaryExpression(location, operation,
		                                  expression, parseMulExpression());
		//location = ts.get().location;
	}

	return expression;
}

Expression* Parser::parseMulExpression() {
	Location location = ts.get().location;

	Expression* expression = parsePrimaryExpression();
	
	while (ts.get().type == Token::STAR || ts.get().type == Token::SLASH) {
		BinaryExpression::Operation operation;

		switch (ts.get().type) {
		case Token::STAR:
			operation = BinaryExpression::MUL;
			break;
		case Token::SLASH:
			operation = BinaryExpression::DIV;
			break;
		default:
			assert(false);
		}

		ts.next();

		expression = new BinaryExpression(location, operation,
		                                  expression, parsePrimaryExpression());
		//location = ts.get().location;
	}

	return expression;
}

Expression* Parser::parsePrimaryExpression() {
	const Location location = ts.get().location;

	Expression* expression = 0;
	
	switch (ts.get().type) {
	case Token::IDENTIFIER:
		expression = new IdentifierExpression(location, parseIdentifier());
		break;

	case Token::NUMBER: {
		const int_t number = ts.get().number;
		ts.next();
		expression = new LiteralNumberExpression(location, number);
		break;
	}

	case Token::KEYWORD_VOID:
		ts.next(); 
		expression = new VoidExpression(location);
		break;

	case Token::LPAREN:
		ts.next();
		expression = parseExpression();
		assumeNext(Token::RPAREN);
		break;

	case Token::LBRACE:
		expression = parseBlockExpression();
		break;

	case Token::KEYWORD_IF:
		expression = parseIfElseExpression();
		break;

	default:
		expectedError("primary expression");
		assert(false);
	}

	expression = parsePostExpression(expression);

	return expression;
}

Expression* Parser::parsePostExpression(Expression* expression) {
	const Location location = ts.get().location;

	switch (ts.get().type) {
	case Token::LPAREN: {
		ts.next();

		CallExpression::argument_list_t arguments;
		
		while (ts.get().type != Token::RPAREN) {
			arguments.push_back(ExpressionPtr(parseExpression()));	

			if (ts.get().type == Token::COMMA) {
				ts.next();

				if (ts.get().type == Token::RPAREN)
					error("bogus comma at end of parameter list");
			}
		}

		assumeNext(Token::RPAREN);

		return new CallExpression(location, expression, arguments);
	}

	default:
		return expression;
	}
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

} // namespace parser
} // namespace llang
