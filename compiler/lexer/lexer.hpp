#ifndef LLANG_LEXER_LEXER_HPP_INCLUDED
#define LLANG_LEXER_LEXER_HPP_INCLUDED

#include <string>

#include "lexer/token.hpp"

namespace llang {
namespace lexer {

class Lexer {
public:
	Lexer(const std::string& filename, const std::string& source)
		: filename(filename), source(source), c(source.c_str()),
		  endOfFile(false), line(1), column(1) {
	}

	Token lexToken();

private:
	const std::string filename;
	const std::string source;

	const char* c; // pointer into source

	bool endOfFile; // end of file reached?

	int line, column; // current line and column in source
};

} // namespace lexer
} // namespace llang

#endif
