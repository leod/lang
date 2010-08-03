#ifndef LLANG_SEMANTIC_VISITOR_INCLUDED_HPP
#define LLANG_SEMANTIC_VISITOR_INCLUDED_HPP

#include "common/context.hpp"
#include "ast/visitor.hpp"
#include "ast/type_ptr.hpp"
#include "ast/decl_ptr.hpp"
#include "ast/expr_ptr.hpp"
#include "semantic/scope_state.hpp"
#include "semantic/scope.hpp"

namespace llang {
namespace semantic {

class Visitors {
public:
	typedef ast::Visitor<ScopeState, ast::TypePtr> TypeVisitor;
	typedef ast::Visitor<ScopeState, ast::DeclPtr> DeclVisitor;
	typedef ast::Visitor<ScopeState, ast::ExprPtr> ExprVisitor;

	scoped_ptr<TypeVisitor> typeVisitor;
	scoped_ptr<DeclVisitor> declVisitor;
	scoped_ptr<ExprVisitor> exprVisitor;

	Visitors(TypeVisitor* typeVisitor,
	         DeclVisitor* declVisitor,
	         ExprVisitor* exprVisitor)
		: typeVisitor(typeVisitor),
		  declVisitor(declVisitor),
		  exprVisitor(exprVisitor) {
	}
};

template <typename Result> class VisitorBase
	: public ast::Visitor<ScopeState, Result> {
protected:
	Visitors* visitors;
	Context& context;

	VisitorBase(Context& context) : context(context) {}

	ast::TypePtr accept(ast::TypePtr n, const ScopeState& p) {
		return visitors->typeVisitor->accept(n, p);
	}

	ast::DeclPtr accept(ast::DeclPtr n, const ScopeState& p) {
		return visitors->declVisitor->accept(n, p);
	}

	ast::ExprPtr accept(ast::ExprPtr n, const ScopeState& p) {
		return visitors->exprVisitor->accept(n, p);
	}

	template<typename T> void acceptOn(shared_ptr<T>& n, const ScopeState& p) {
		n = accept(n, p);
	}

	template<typename T> void acceptOn(T begin, T end, const ScopeState& p) {
		for (; begin != end; ++begin)
			acceptOn(*begin, p);
	}

	void acceptScope(Scope* scope, ScopeState state) {
		state.scope = scope;

		for (auto it = scope->decls.begin();
		     it != scope->decls.end();
		     ++it) {
			acceptOn(it->second, state);
		}
	}
};

} // namespace semantic
} // namespace llang

#endif
