#include "lexer/lexer.hpp"

using namespace llang;

int main() {
	lexer::Lexer lexer("test", "()");

	bool endOfFile = false;
	do
	{
		lexer::Token token = lexer.lexToken();
		endOfFile = (token.type == lexer::Token::END_OF_FILE);
	} while(!endOfFile);
}
