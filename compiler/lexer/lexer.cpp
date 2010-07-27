#include <stdexcept>

#include "lexer/lexer.hpp"

namespace llang {
namespace lexer {

Token Lexer::lexToken() {
	if (endOfFile)
		throw std::runtime_error("end of file reached");

	Location location(filename, line, column);
	
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

} // namespace llang
} // namespace lexer
