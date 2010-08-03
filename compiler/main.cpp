#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>

#include "util/smart_ptr.hpp"
#include "common/diagnostics.hpp"
#include "common/config.hpp"
#include "common/context.hpp"

#include "lexer/lexer.hpp"
#include "lexer/token_stream.hpp"

#include "ast/decl.hpp"
#include "ast/type.hpp"
#include "ast/expr.hpp"
#include "parser/parser.hpp"

#include "semantic/phase1/visitors.hpp"
#include "semantic/phase2/visitors.hpp"

#include "codegen/llvm/codegen.hpp"

using namespace llang;

int main(int argc, const char** argv) {
	std::string filename;
	if (argc == 1)
		filename = "test.ll";
	else if (argc == 2)
		filename = argv[1];
	else
		throw std::runtime_error("wrong number of parameters");

	std::string code;

	{
		std::fstream ifs(filename.c_str());

		if (ifs.fail())
			throw std::runtime_error("couldn't read file " + filename);

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
	ast::DeclPtr module(parser.parseModule());
	//print(*module);

	scoped_ptr<semantic::Visitors>
		phase1(semantic::makePhase1Visitors(context)),
		phase2(semantic::makePhase2Visitors(context));
	semantic::ScopeState state;

	module = phase1->declVisitor->accept(module, state);
	module = phase2->declVisitor->accept(module, state);

	codegen::Codegen gen(context,
		ast::assumeIsA<ast::Module>(module));
	gen.run();
}
