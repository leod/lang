#include <cstdarg> 
#include <cstdio>

#include "ast/type.hpp"
#include "ast/decl.hpp"
#include "ast/expr.hpp"
#include "parser/parser.hpp"

namespace llang {
namespace parser {

using namespace ast;
using namespace lexer;

ModulePtr Parser::parseModule() {
	Module::DeclList decls;

	while (ts.get().type != Token::END_OF_FILE) {
		decls.push_back(parseDecl());
		assumeNext(Token::SEMICOLON);
	}

	return ModulePtr(new Module(Location(moduleName, 1, 1), moduleName, decls));
}

DeclPtr Parser::parseDecl() {
	switch (ts.get().type) {
	case Token::KEYWORD_FN:
		return parseFunctionDecl();

	case Token::KEYWORD_VAR:
		return parseVariableDecl();

	case Token::KEYWORD_EXTERN: {
		ts.next();
		// TODO: wrong location

		FunctionDeclPtr function = assumeIsA<FunctionDecl>(parseFunctionDecl());	
		function->isExtern = true;
		
		if (function->body) {
			diag.error(function->location(),
				"extern function '%s' cannot have body",
				function->name.c_str());
		}

		return function;
	}
		
	default:
		expectedError("decl");		
	}

	assert(false);
}

TypePtr Parser::parseType() {
	Location location = ts.get().location;

	switch (ts.get().type) {
	case Token::KEYWORD_I32: {
		IntegralType::Kind type;

		type = IntegralType::I32;
		goto Cintegral;

	case Token::KEYWORD_CHAR:
		type = IntegralType::CHAR;
		goto Cintegral;

	case Token::KEYWORD_VOID:
		type = IntegralType::VOID;
		goto Cintegral;

	case Token::KEYWORD_STRING:
		type = IntegralType::STRING;
		goto Cintegral;

	case Token::KEYWORD_BOOL:
		type = IntegralType::BOOL;
		goto Cintegral;
	
	Cintegral:
		ts.next();
		return TypePtr(new IntegralType(location, type));
	}

	default:
		expectedError("type");
		assert(false);
	}
}

ExprPtr Parser::parseExpr() {
	switch (ts.get().type) {
	default:
		return parseAssignExpr();

	case Token::KEYWORD_FN:
	case Token::KEYWORD_VAR:
		const Location location = ts.get().location;
		return ExprPtr(new DeclExpr(location, parseDecl()));
	}
}

DeclPtr Parser::parseFunctionDecl() {
	const Location location = ts.get().location;

	TypePtr returnType;
	identifier_t name;
	FunctionDecl::ParameterList parameters;
	parseFunctionPrototype(returnType, name, parameters);

	ExprPtr body;

	if (ts.get().type == Token::EQUALS) {
		ts.next();
		body = parseExpr();
	}

	return DeclPtr(new FunctionDecl(location, name, returnType, parameters,
	                                body));
}

DeclPtr Parser::parseVariableDecl() {
	const Location location = ts.get().location;

	assumeNext(Token::KEYWORD_VAR);
	
	// TODO: parse inferred decls
	TypePtr type = parseType();

	identifier_t identifier = parseIdentifier();

	if (ts.get().type == Token::SEMICOLON) {
		assert(false); // TODO: implement vars without initializers
	} else {
		assumeNext(Token::EQUALS);

		ExprPtr initializer = parseExpr();

		return DeclPtr(new VariableDecl(location, identifier, type,
		                                initializer));
	}
}

ExprPtr Parser::parseBlockExpr() {
	const Location location = ts.get().location;

	assumeNext(Token::LBRACE);

	BlockExpr::ExprList exprs;

	while (ts.get().type != Token::RBRACE) {
		exprs.push_back(ExprPtr(parseExpr()));
		assumeNext(Token::SEMICOLON);
	}

	assumeNext(Token::RBRACE);

	return ExprPtr(new BlockExpr(location, exprs));
}

ExprPtr Parser::parseIfElseExpr() {
	const Location location = ts.get().location;

	assumeNext(Token::KEYWORD_IF);

	assumeNext(Token::LPAREN);
	ExprPtr condition = parseExpr();
	assumeNext(Token::RPAREN);

	ExprPtr ifBody = parseExpr();

	assumeNext(Token::KEYWORD_ELSE);
	ExprPtr elseBody = parseExpr();

	return ExprPtr(new IfElseExpr(location, condition, ifBody, elseBody));
}

ExprPtr Parser::parseAssignExpr() {
	Location location = ts.get().location;

	ExprPtr expr = parseEqualsExpr();
	
	// TODO

	return expr;
}

ExprPtr Parser::parseEqualsExpr() {
	Location location = ts.get().location;

	ExprPtr expr = parseAddExpr();
	
	while (ts.get().type == Token::EQUALS) {
		ts.next();

		expr = ExprPtr(new BinaryExpr(location, BinaryExpr::EQUALS, expr,
		                              parseAddExpr()));
	}

	return expr;
}

ExprPtr Parser::parseAddExpr() {
	Location location = ts.get().location;

	ExprPtr expr = parseMulExpr();
	
	while (ts.get().type == Token::PLUS || ts.get().type == Token::MINUS) {
		BinaryExpr::Operation operation;

		switch (ts.get().type) {
		case Token::PLUS:
			operation = BinaryExpr::ADD;
			break;
		case Token::MINUS:
			operation = BinaryExpr::SUB;
			break;
		default:
			assert(false);
		}

		ts.next();

		expr = ExprPtr(new BinaryExpr(location, operation, expr,
		                              parseMulExpr()));
		//location = ts.get().location;
	}

	return expr;
}

ExprPtr Parser::parseMulExpr() {
	Location location = ts.get().location;

	ExprPtr expr = parsePrimaryExpr();
	
	while (ts.get().type == Token::STAR || ts.get().type == Token::SLASH) {
		BinaryExpr::Operation operation;

		switch (ts.get().type) {
		case Token::STAR:
			operation = BinaryExpr::MUL;
			break;
		case Token::SLASH:
			operation = BinaryExpr::DIV;
			break;
		default:
			assert(false);
		}

		ts.next();

		expr = ExprPtr(new BinaryExpr(location, operation, expr,
		                              parsePrimaryExpr()));
		//location = ts.get().location;
	}

	return expr;
}

ExprPtr Parser::parsePrimaryExpr() {
	const Location location = ts.get().location;

	ExprPtr expr;
	
	switch (ts.get().type) {
	case Token::IDENTIFIER:
		expr = ExprPtr(new IdentifierExpr(location, parseIdentifier()));
		break;

	case Token::NUMBER: {
		const int_t number = ts.get().number;
		ts.next();
		expr = ExprPtr(new LiteralNumberExpr(location, number));
		break;
	}

	case Token::STRING: {
		const std::string string = ts.get().identifier;
		ts.next();
		expr = ExprPtr(new LiteralStringExpr(location, string));
		break;
	}

	case Token::KEYWORD_TRUE:
	case Token::KEYWORD_FALSE: {
		const Token::Type type = ts.get().type;
		ts.next();
		expr = ExprPtr(
			new LiteralBoolExpr(location, type == Token::KEYWORD_TRUE));
		break;
	}

	case Token::KEYWORD_VOID:
		ts.next(); 
		expr = ExprPtr(new VoidExpr(location));
		break;

	case Token::LPAREN:
		ts.next();
		expr = parseExpr();
		assumeNext(Token::RPAREN);
		break;

	case Token::LBRACE:
		expr = parseBlockExpr();
		break;

	case Token::KEYWORD_IF:
		expr = parseIfElseExpr();
		break;

	default:
		expectedError("primary expr");
		assert(false);
	}

	expr = parsePostExpr(expr);

	return expr;
}

ExprPtr Parser::parsePostExpr(ExprPtr expr) {
	const Location location = ts.get().location;

	switch (ts.get().type) {
	case Token::LPAREN: {
		ts.next();

		CallExpr::ArgumentList arguments;
		
		while (ts.get().type != Token::RPAREN) {
			arguments.push_back(ExprPtr(parseExpr()));	

			if (ts.get().type == Token::COMMA) {
				ts.next();

				if (ts.get().type == Token::RPAREN)
					error("bogus comma at end of parameter list");
			}
		}

		assumeNext(Token::RPAREN);

		return parsePostExpr(ExprPtr(new CallExpr(location, expr, arguments)));
	}

	case Token::LBRACKET: {
		ts.next();

		ExprPtr index = parseExpr();
		assumeNext(Token::RBRACKET);

		return parsePostExpr(ExprPtr(
			new ArrayElementExpr(location, expr, index)));
	}

	default:
		return expr;
	}
}

identifier_t Parser::parseIdentifier() {
	assume(Token::IDENTIFIER);
	const identifier_t identifier = ts.get().identifier;
	ts.next();

	return identifier;
}

void Parser::parseFunctionPrototype(TypePtr& returnType, identifier_t& name,
                                    FunctionDecl::ParameterList& parameters) {
	assumeNext(Token::KEYWORD_FN);
	
	returnType = parseType();	
	name = parseIdentifier();

	assumeNext(Token::LPAREN);

	if (ts.get().type != Token::RPAREN) {
		bool doLoop = true;

		do {
			const Location location = ts.get().location;

			TypePtr type = parseType();

			bool hasName = false;	
			identifier_t name;

			if (ts.get().type == Token::IDENTIFIER) {
				hasName = true;
				name = parseIdentifier();
			}

			parameters.push_back(ParameterDeclPtr(
				new ParameterDecl(location, name, hasName, type)));	

			if (ts.get().type != Token::COMMA)
				doLoop = false;
			else
				ts.next();
		} while(doLoop);
	}

	assumeNext(Token::RPAREN);
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
