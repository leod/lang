#ifndef LLANG_AST_PRINT_VISITOR_HPP_INCLUDED
#define LLANG_AST_PRINT_VISITOR_HPP_INCLUDED

#include "ast/type.hpp"
#include "ast/declaration.hpp"
#include "ast/expression.hpp"
#include "ast/visitor.hpp"

namespace llang {
namespace ast {

class PrintVisitor : public Visitor<size_t, void> {
public:
	PrintVisitor() { }

protected:
	virtual void visit(Node&, int) {
		assert(false);
	}
};

} // namespace ast
} // namespace llang

#endif
