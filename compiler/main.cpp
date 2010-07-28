#include <iostream>
#include <sstream>
#include <fstream>

#include "common/diagnostics.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token_stream.hpp"
#include "ast/print_visitor.hpp"
#include "ast/declaration.hpp"
#include "parser/parser.hpp"

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

	Diagnostics diag;

	lexer::Lexer lexer(diag, filename, code);
	lexer::TokenStream ts(lexer);

	parser::Parser parser(diag, filename, ts);
	ast::Module* module = parser.parseModule();
	assert(module);
}
