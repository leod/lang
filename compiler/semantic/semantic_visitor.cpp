#include <cassert>

#include "semantic/semantic_visitor.hpp"
#include "semantic/symbol.hpp"
#include "semantic/expression.hpp"
#include "semantic/type.hpp"

namespace llang {
namespace semantic {

template <typename Result> class VisitorBase
	: public Visitor<ScopeState, Result> {
protected:
	SemanticVisitors* visitors;
	Context& context;

	VisitorBase(Context& context) : context(context) {}
	friend SemanticVisitors* makeSemanticVisitors(Context&);

	TypePtr accept(TypePtr n, const ScopeState& p) {
		return visitors->typeVisitor->accept(n, p);
	}

	SymbolPtr accept(SymbolPtr n, const ScopeState& p) {
		return visitors->symbolVisitor->accept(n, p);
	}

	ExpressionPtr accept(ExpressionPtr n, const ScopeState& p) {
		return visitors->expressionVisitor->accept(n, p);
	}
};

class TypeVisitor : public VisitorBase<TypePtr> {
private:
	TypeVisitor(Context& context)
		: VisitorBase<TypePtr>(context) {}
	friend SemanticVisitors* makeSemanticVisitors(Context&);

protected:
};

class SymbolVisitor : public VisitorBase<SymbolPtr> {
private:
	SymbolVisitor(Context& context)
		: VisitorBase<SymbolPtr>(context) {}
	friend SemanticVisitors* makeSemanticVisitors(Context&);

protected:
};

class ExpressionVisitor : public VisitorBase<ExpressionPtr> {
private:
	ExpressionVisitor(Context& context)
		: VisitorBase<ExpressionPtr>(context) {}
	friend SemanticVisitors* makeSemanticVisitors(Context&);

protected:
};

SemanticVisitors* makeSemanticVisitors(Context& context) {
	TypeVisitor* typeVisitor = new TypeVisitor(context);
	SymbolVisitor* symbolVisitor = new SymbolVisitor(context);
	ExpressionVisitor* expressionVisitor = new ExpressionVisitor(context);

	SemanticVisitors* visitors = new SemanticVisitors(typeVisitor,
	                                                  symbolVisitor,
	                                                  expressionVisitor);
	
	typeVisitor->visitors = visitors;
	symbolVisitor->visitors = visitors;
	expressionVisitor->visitors = visitors;

	return visitors;
}

} // namespace semantic
} // namespace llang

