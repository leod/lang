#include <iostream>
#include <sstream>
#include <fstream>

#include "util/smart_ptr.hpp"
#include "common/diagnostics.hpp"
#include "common/config.hpp"
#include "common/context.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token_stream.hpp"
#include "ast/print_visitor.hpp"
#include "ast/declaration.hpp"
#include "ast/type.hpp"
#include "ast/expression.hpp"
#include "parser/parser.hpp"
#include "semantic/symbol.hpp"
#include "semantic/ast_visitor.hpp"
#include "semantic/semantic_visitor.hpp"

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
	scoped_ptr<ast::Module> module(parser.parseModule());

	scoped_ptr<semantic::AstVisitors> astVisitors(
		semantic::makeAstVisitors(context));
	semantic::ScopeState state;

	semantic::SymbolPtr semModule(
		astVisitors->declarationVisitor->accept(*module, state));

	scoped_ptr<semantic::SemanticVisitors> semanticVisitors(
		semantic::makeSemanticVisitors(context));
	semModule = semanticVisitors->symbolVisitor->accept(semModule, state);
}
