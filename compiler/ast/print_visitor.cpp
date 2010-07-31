#include <cstdarg>

#include "ast/type.hpp"
#include "ast/declaration.hpp"
#include "ast/expression.hpp"
#include "ast/print_visitor.hpp"

namespace llang {
namespace ast {

class PrintVisitor : public Visitor<void, void> {
public:
	PrintVisitor()
		: depth(0) {
	}

protected:
	virtual void visit(Module& m) {
		print("module %s", m.name.c_str());

		IncDepth guard(depth);

		for (Module::declaration_list_t::iterator it = m.declarations.begin();
		     it != m.declarations.end();
		     ++it) {
			accept(**it);
		}
	}

	virtual void visit(FunctionDeclaration& fn) {
		print("fn %s", fn.name.c_str());

		IncDepth guard(depth);

		print("return type:"); acceptWithDepth(*fn.returnType);

		for (FunctionDeclaration::parameter_list_t::iterator it =
		         fn.parameters.begin();
		     it != fn.parameters.end();
		     ++it) {
			print("param %s", it->name.c_str());
			{
				IncDepth guard(depth);

				print("type:"); acceptWithDepth(*it->type);
			}
		}

		print("body:");
		acceptWithDepth(*fn.body);
	}

	virtual void visit(VariableDeclaration& var) {
		print("var %s", var.name.c_str());

		IncDepth guard(depth);

		print("type:"); acceptWithDepth(*var.type);
		print("initializer:"); acceptWithDepth(*var.initializer);
	}

	virtual void visit(LiteralNumberExpression& num) {
		print("literal: %d", num.number);
	}

	virtual void visit(IfElseExpression& ifElse) {
		print("if/else");

		IncDepth guard(depth);

		print("condition:"); acceptWithDepth(*ifElse.condition);
		print("if expr:"); acceptWithDepth(*ifElse.ifExpression);
		print("else expr:"); acceptWithDepth(*ifElse.elseExpression);
	}

	virtual void visit(BlockExpression& block) {
		print("block");

		IncDepth guard(depth);

		for (BlockExpression::expression_list_t::iterator it =
		         block.expressions.begin();
		     it != block.expressions.end();
	         ++it) {
			accept(**it);	
		}
	}

	virtual void visit(BinaryExpression& b) {
		std::string type;
		switch (b.operation) {
		case BinaryExpression::ADD:
			type = "+";
			break;
		case BinaryExpression::MUL:
			type = "*";
			break;

		default:
			assert(false);
		}

		print("binary expression: %s", type.c_str());
		{
			IncDepth guard(depth);
			print("left: "); acceptWithDepth(*b.left);
			print("right: "); acceptWithDepth(*b.right);
		}
	}

	virtual void visit(IdentifierExpression& i) {
		print("identifier: %s", i.name.c_str());
	}

	virtual void visit(CallExpression& c) {
		print("call expression");

		IncDepth guard(depth);

		print("callee"); acceptWithDepth(*c.callee);

		for (CallExpression::argument_list_t::iterator it = c.arguments.begin();
		     it != c.arguments.end();
		     ++it) {
			print("argument"); acceptWithDepth(**it);
		}
	}

	virtual void visit(DeclarationExpression& d) {
		accept(*d.declaration);	
	}

	virtual void visit(IntegralType& type) {
		switch (type.type) {
		case lexer::Token::KEYWORD_I32:
			print("i32");
			break;
		case lexer::Token::KEYWORD_VOID:
			print("void");
			break;
		default:
			assert(false);	
		}
	}

private:
	size_t depth;

	struct IncDepth {
		size_t& d;

		IncDepth(size_t& d)
			: d(d) {
			++d;
		}

		~IncDepth() {
			--d;
		}
	};
	
	void print(const char* format, ...) {
		for (size_t i = 0; i < depth; ++i) printf("\t");

		va_list argp;
		va_start(argp, format);
		vfprintf(stdout, format, argp);
		printf("\n");
		va_end(argp);
	}

	void acceptWithDepth(Node& node) {
		IncDepth guard(depth);
		accept(node);
	}
};

Visitor<void, void>* makePrintVisitor() {
	return new PrintVisitor;
}

} // namespace ast
} // namespace llang
