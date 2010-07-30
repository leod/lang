#include <iostream>
#include <sstream>
#include <fstream>

#include "common/diagnostics.hpp"
#include "common/config.hpp"
#include "common/context.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token_stream.hpp"
#include "ast/print_visitor.hpp"
#include "ast/declaration.hpp"
#include "parser/parser.hpp"
#include "semantic/ast_visitor.hpp"

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

	Config config;
	Diagnostics diag(config);
	Context context(config, diag);

	lexer::Lexer lexer(context, filename, code);
	lexer::TokenStream ts(lexer);

	parser::Parser parser(context, filename, ts);
	boost::scoped_ptr<ast::Module> module(parser.parseModule());

	ast::PrintVisitor printer;
	printer.accept(*module);
}
