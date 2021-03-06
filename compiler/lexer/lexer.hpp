#ifndef LLANG_LEXER_LEXER_HPP_INCLUDED
#define LLANG_LEXER_LEXER_HPP_INCLUDED

#include <string>

#include "common/context.hpp"
#include "lexer/token.hpp"

namespace llang {
namespace lexer {

class Lexer {
public:
	Lexer(Context& context,
	      const std::string& filename,
	      const std::string& source)
		: diag(context.diag), filename(filename), source(source),
		  c(source.c_str()), lineStart(c), endOfFile(false), line(1) {
	}

	Token lexToken();

private:
	Token lexIdentifier(const Location& location);
	Token lexNumber(const Location& location);
	Token lexStringLiteral(const Location& location);

	bool eatWhitespace();
	bool eatComments();

	Diagnostics& diag;

	const std::string filename;
	const std::string source;

	const char* c; // pointer into source
	const char* lineStart;

	bool endOfFile; // end of file reached?

	size_t line; // current line in source
};

} // namespace lexer
} // namespace llang

#endif
