#include <stdexcept>
#include <cassert>
#include <cctype>
#include <map>
#include <sstream>

#include "lexer/lexer.hpp"

namespace llang {
namespace lexer {

typedef std::map<identifier_t, Token::Type> keyword_map_t;

keyword_map_t makeKeywordMap()
{
	keyword_map_t map;

	map["fn"] = Token::KEYWORD_FN;
	map["var"] = Token::KEYWORD_VAR;
	map["if"] = Token::KEYWORD_IF;
	map["else"] = Token::KEYWORD_ELSE;
	map["i32"] = Token::KEYWORD_I32;
	map["int"] = Token::KEYWORD_INT;
	map["void"] = Token::KEYWORD_VOID;
	map["string"] = Token::KEYWORD_STRING;
	map["char"] = Token::KEYWORD_CHAR;
	map["bool"] = Token::KEYWORD_BOOL;
	map["true"] = Token::KEYWORD_TRUE;
	map["false"] = Token::KEYWORD_FALSE;
	map["extern"] = Token::KEYWORD_EXTERN;
	map["arr"] = Token::KEYWORD_ARRAY;

	return map;
}

const keyword_map_t keywords = makeKeywordMap();

bool isKeyword(const identifier_t& identifier,
               Token::Type& outType) {
	keyword_map_t::const_iterator elemIterator = keywords.find(identifier);

	if (elemIterator == keywords.end())
		return false;

	outType = elemIterator->second;
	return true;
}

Token Lexer::lexToken() {
	if (endOfFile)
		throw std::runtime_error("end of file reached");

	while (eatWhitespace() || eatComments());

	Location location(filename, line, static_cast<size_t>(c - lineStart) + 1);
	
	switch (*c) {
		// TODO: this repetiton kind of sucks
		case '(':
			++c;
			return Token(location, Token::LPAREN);	
		case ')':
			++c;
			return Token(location, Token::RPAREN);	
		case '=':
			++c;
			return Token(location, Token::EQUALS);	
		case ';':
			++c;
			return Token(location, Token::SEMICOLON);
		case ',':
			++c;
			return Token(location, Token::COMMA);
		case '{':
			++c;
			return Token(location, Token::LBRACE);
		case '}':
			++c;
			return Token(location, Token::RBRACE);
		case '+':
			++c;
			return Token(location, Token::PLUS);
		case '*':
			++c;
			return Token(location, Token::STAR);
		case '-':
			++c;
			return Token(location, Token::MINUS);
		case '/':
			++c;
			return Token(location, Token::SLASH);
		case '[':
			++c;
			return Token(location, Token::LBRACKET);
		case ']':
			++c;
			return Token(location, Token::RBRACKET);
		case '"':
			return lexStringLiteral(location);
		case '\0':
			endOfFile = true;
			return Token(location, Token::END_OF_FILE);
		default:
			if (isalpha(*c) || *c == '_')
				return lexIdentifier(location);
			else if (isdigit(*c))
				return lexNumber(location);

			// TODO: error
			assert(false);
	}

	assert(false);
}

Token Lexer::lexIdentifier(const Location& location) {
	assert(isalpha(*c) || *c == '_');

	const char* start = c;
	size_t length = 0;

	while (isalpha(*c) || *c == '_' || isdigit(*c)) {
		++length;
		++c;
	}

	std::string identifier(start, length);
	Token::Type tokenType;

	if (isKeyword(identifier, tokenType))
		return Token(location, tokenType);
	else
		return Token(location, identifier);
}

Token Lexer::lexNumber(const Location& location) {
	assert(isdigit(*c));

	int_t number = 0;

	while (isdigit(*c)) {
		int_t digit = static_cast<int>(*c - '0');
		number = number * 10 + digit;
			
		++c;
	}

	return Token(location, number);
}

Token Lexer::lexStringLiteral(const Location& location) {
	assert(*c == '"');
	++c;

	std::stringstream ss;

	// TODO: escape sequences
	while (*c != '"') {
		if (*c == '\0') {
			diag.error(location, "unterminated string literal");
		} else if (*c != '\\') {
			ss << *c++;
		} else  {
			++c;
			
			switch (*c) {
			case '\\': ss << '\\'; break;
			case 'n': ss << '\n'; break;
			case '"': ss << '"'; break;
			case '0': ss << '\0'; break;
			default: diag.error(location, "unrecognized escape sequence");
			}

			++c;
		}
	} 

	++c;

	std::string string(ss.str());

	return Token(location, string, Token::STRING);
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

	return start != c;
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
