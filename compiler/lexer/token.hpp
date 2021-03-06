#ifndef LLANG_LEXER_TOKEN_HPP_INCLUDED
#define LLANG_LEXER_TOKEN_HPP_INCLUDED

#include <iostream>

#include "common/number.hpp"
#include "common/identifier.hpp"
#include "common/location.hpp"

namespace llang {
namespace lexer {

struct Token {
	const Location location;

	const enum Type {
		NUMBER,
		LPAREN,
		RPAREN,
		EQUALS,
		IDENTIFIER,
		SEMICOLON,
		COMMA,
		LBRACE,
		RBRACE,
		PLUS,
		STAR,
		MINUS,
		SLASH,
		STRING,
		LBRACKET,
		RBRACKET,
		END_OF_FILE,

		KEYWORD_FN,
		KEYWORD_VAR,
		KEYWORD_IF,
		KEYWORD_ELSE,
		KEYWORD_I32,
		KEYWORD_INT,
		KEYWORD_CHAR,
		KEYWORD_VOID,
		KEYWORD_STRING,
		KEYWORD_BOOL,
		KEYWORD_TRUE,
		KEYWORD_FALSE,
		KEYWORD_EXTERN,
		KEYWORD_ARRAY,

		ENUM_MAX
	} type;

	const int_t number; // set if type == NUMBER
	const identifier_t identifier; // set if type == IDENTIFIER

	Token(const Location& location, Type type)
		: location(location), type(type), number(-666), identifier("error") {
	}

	Token(const Location& location, const int_t& number)
		: location(location), type(NUMBER), number(number),
		  identifier("error") {
	}

	Token(const Location& location, const identifier_t& identifier,
	      Type type = IDENTIFIER)
		: location(location), type(type), number(-666),
		  identifier(identifier) {
	}

	static const char* typeToString(Type type);
};

std::ostream& operator<<(std::ostream& os, const Token& token);

} // namespace lexer
} // namespace llang

#endif
