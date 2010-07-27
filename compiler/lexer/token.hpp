#ifndef LLANG_LEXER_TOKEN_HPP_INCLUDED
#define LLANG_LEXER_TOKEN_HPP_INCLUDED

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
		END_OF_FILE,

		KEYWORD_FN,
		KEYWORD_VAR,
		KEYWORD_IF,
		KEYWORD_ELSE
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

	Token(const Location& location, const identifier_t& identifier)
		: location(location), type(IDENTIFIER), number(-666),
		  identifier(identifier) {
	}
};

} // namespace lexer
} // namespace llang

#endif
