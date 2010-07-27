#include "lexer/token.hpp"

namespace llang {
namespace lexer {

const char* typeToString[] =
	{ "number",
	  "lparen",
	  "rparen",
	  "equals",
	  "identifier",
	  "semicolon",
	  "end_of_file",
	  "fn",
	  "var",
	  "if",
	  "else"
	};

std::ostream& operator<<(std::ostream& os, const Token& token) {
	os << token.location << ":" << typeToString[token.type];

	if (token.type == Token::NUMBER)
		os << ":" << token.number;
	else if (token.type == Token::IDENTIFIER)
		os << ":" << token.identifier;

	return os;
}

} // namespace lexer
} // namespace llang
