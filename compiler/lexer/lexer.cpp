#include <stdexcept>

#include "lexer/lexer.hpp"

namespace llang {
namespace lexer {

Token Lexer::lexToken() {
	if (endOfFile)
		throw std::runtime_error("end of file reached");

	while (eatWhitespace() || eatComments());

	Location location(filename, line, static_cast<size_t>(c - lineStart) + 1);
	
	switch (*c) {
		case '(':
			++c;
			return Token(location, Token::LPAREN);	
		case ')':
			++c;
			return Token(location, Token::RPAREN);	
		case '=':
			++c;
			return Token(location, Token::EQUALS);	
		case '\0':
			endOfFile = true;
			return Token(location, Token::END_OF_FILE);
	}
}

bool Lexer::eatWhitespace() {
	const char* start = c;

	while (*c == ' ' || *c == '\t' || *c == '\n') {
		if (*c == '\n') { // TODO: CRLF
			++line;
			lineStart = c + 1;
		}
		++c;
	}
}

bool Lexer::eatComments() {
	if (*c == '/' && *(c + 1) == '/') {
		while(*++c != '\n');	
		return true;
	}
	
	return false;
}

} // namespace llang
} // namespace lexer
