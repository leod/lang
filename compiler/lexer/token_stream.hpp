#ifndef LLANG_LEXER_TOKEN_STREAM_HPP_INCLUDED
#define LLANG_LEXER_TOKEN_STREAM_HPP_INCLUDED

#include <deque>
#include <cassert>

#include "lexer/token.hpp"

namespace llang {
namespace lexer {

class TokenStream {
public:
	TokenStream(Lexer& lexer)
		: lexer(lexer) {
	}

	const Token& get() const {
		assert(tokens.size() > 0);
		return tokens.front();
	}

	const Token& next() {
		if (tokens.size() > 0) tokens.pop_front();
		if (tokens.size() < 1) addOneToken();
		return get();
	}

	const Token& peek(const size_t distance = 1) {
		assert(distance >= 1);

		if (tokens.size() <= distance) {
			size_t delta = distance - tokens.size();
			for (size_t i = 0; i <= delta; ++i) {
				addOneToken();
			}

			assert(tokens.size() == distance + 1);
		}

		return tokens[distance];
	}

private:
	Lexer& lexer;

	void addOneToken() { tokens.push_back(lexer.lexToken()); }

	std::deque<Token> tokens;
};

} // namespace lexer
} // namespace llang

#endif

