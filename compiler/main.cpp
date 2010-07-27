#include <iostream>
#include <sstream>
#include <fstream>

#include "lexer/lexer.hpp"

using namespace llang;

int main() {
	std::string filename = "test.ll";
	std::string code;

	{
		std::fstream ifs(filename.c_str());
		std::stringstream oss;
		oss << ifs.rdbuf();
		code = oss.str();
	}

	lexer::Lexer lexer(filename, code);

	bool endOfFile = false;
	do
	{
		lexer::Token token = lexer.lexToken();
		std::cout << token << std::endl;

		endOfFile = (token.type == lexer::Token::END_OF_FILE);
	} while(!endOfFile);
}
