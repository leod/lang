#include "lexer/token.hpp"

namespace llang {
namespace lexer {

const char* typeToStringArray[] =
	{ "number",
	  "lparen",
	  "rparen",
	  "equals",
	  "identifier",
	  "semicolon",
	  "comma",
	  "lbrace",
	  "rbrace",
	  "end_of_file",
	  "fn",
	  "var",
	  "if",
	  "i32",
	  "else",
	  "void"
	};

const char* Token::typeToString(Token::Type type) {
	return typeToStringArray[type];
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
	os << token.location << ":" << Token::typeToString(token.type);

	if (token.type == Token::NUMBER)
		os << ":" << token.number;
	else if (token.type == Token::IDENTIFIER)
		os << ":" << token.identifier;

	return os;
}

} // namespace lexer
} // namespace llang
