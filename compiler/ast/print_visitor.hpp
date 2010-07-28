#ifndef LLANG_AST_PRINT_VISITOR_HPP_INCLUDED
#define LLANG_AST_PRINT_VISITOR_HPP_INCLUDED

#include "ast/type.hpp"
#include "ast/declaration.hpp"
#include "ast/expression.hpp"
#include "ast/visitor.hpp"

namespace llang {
namespace ast {

class PrintVisitor : public Visitor<void, void> {
public:
	PrintVisitor()
		: depth(0) {
	}

protected:
	virtual void visit(Node&) {
		assert(false);
	}

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

	}

	virtual void visit(VoidType&) {
		
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
};

} // namespace ast
} // namespace llang

#endif
