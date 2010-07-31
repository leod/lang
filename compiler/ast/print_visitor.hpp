#ifndef LLANG_AST_PRINT_VISITOR_HPP_INCLUDED
#define LLANG_AST_PRINT_VISITOR_HPP_INCLUDED

#include "util/smart_ptr.hpp"
#include "ast/visitor.hpp"

namespace llang {
namespace ast {

Visitor<void, void>* makePrintVisitor();

template <typename T> void print(T& node) {
	scoped_ptr<Visitor<void, void> > visitor(makePrintVisitor());
	visitor->accept(node);
}

} // namespace ast
} // namespace llang

#endif
