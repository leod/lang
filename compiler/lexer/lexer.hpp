#ifndef LLANG_LEXER_LEXER_HPP_INCLUDED
#define LLANG_LEXER_LEXER_HPP_INCLUDED

#include <string>
#include <map>

#include "lexer/token.hpp"

namespace llang {
namespace lexer {

class Lexer {
public:
	Lexer(const std::string& filename, const std::string& source)
		: filename(filename), source(source), c(source.c_str()),
		  lineStart(c), endOfFile(false), line(1) {
	}

	Token lexToken();

private:
	const std::string filename;
	const std::string source;

	const char* c; // pointer into source
	const char* lineStart;

	bool endOfFile; // end of file reached?

	int line; // current line in source

	Token lexIdentifier(const Location& location);
	Token lexNumber(const Location& location);

	bool eatWhitespace();
	bool eatComments();
};

} // namespace lexer
} // namespace llang

#endif
