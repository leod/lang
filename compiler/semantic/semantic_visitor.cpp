#include <cassert>
#include <cstdio>

#include "semantic/semantic_visitor.hpp"
#include "semantic/symbol.hpp"
#include "semantic/expression.hpp"
#include "semantic/type.hpp"

namespace llang {
namespace semantic {

template <typename Result> class SemanticVisitorBase
	: public Visitor<ScopeState, Result> {
protected:
	SemanticVisitors* visitors;
	Context& context;

	SemanticVisitorBase(Context& context) : context(context) {}
	friend SemanticVisitors* makeSemanticVisitors(Context&);

	TypePtr accept(TypePtr n, const ScopeState& p) {
		assert(n);
		assert(visitors && visitors->typeVisitor);
		return visitors->typeVisitor->accept(n, p);
	}

	SymbolPtr accept(SymbolPtr n, const ScopeState& p) {
		assert(n);
		assert(visitors && visitors->symbolVisitor);
		return visitors->symbolVisitor->accept(n, p);
	}

	ExpressionPtr accept(ExpressionPtr n, const ScopeState& p) {
		assert(n);
		assert(visitors && visitors->expressionVisitor);
		return visitors->expressionVisitor->accept(n, p);
	}


	template<typename T> void acceptOn(shared_ptr<T> n, const ScopeState& p) {
		n = accept(n, p);
	}
};


class SemanticTypeVisitor : public SemanticVisitorBase<TypePtr> {
private:
	SemanticTypeVisitor(Context& context)
		: SemanticVisitorBase<TypePtr>(context) {}
	friend SemanticVisitors* makeSemanticVisitors(Context&);

protected:
#define ID_VISIT(type) \
	virtual TypePtr visit(type##Ptr ptr, ScopeState) \
	{ return ptr; }

	//ID_VISIT(IntegralType)

	virtual TypePtr visit(IntegralTypePtr type, ScopeState) {
		return type;
	}

	virtual TypePtr visit(FunctionTypePtr type, ScopeState state) {
		for (auto it = type->parameterTypes.begin();
		     it != type->parameterTypes.end();
			 ++it) {
			acceptOn(*it, state);	
		}

		acceptOn(type->returnType, state);

		return type;
	}

#undef ID_VISIT
};

class SemanticSymbolVisitor : public SemanticVisitorBase<SymbolPtr> {
private:
	SemanticSymbolVisitor(Context& context)
		: SemanticVisitorBase<SymbolPtr>(context) {}
	friend SemanticVisitors* makeSemanticVisitors(Context&);

	void acceptScope(Scope* scope, ScopeState state) {
		state.scope = scope;

		for (auto it = scope->symbols.begin();
		     it != scope->symbols.end();
		     ++it) {
			acceptOn(it->second, state);
		}
	}

protected:
	virtual SymbolPtr visit(ModulePtr module, ScopeState state) {
		acceptScope(module->scope.get(), state);
		return module;
	}

	virtual SymbolPtr visit(VariableSymbolPtr variable, ScopeState state) {
		acceptOn(variable->type, state);
		acceptOn(variable->initializer, state);
		return variable;
	}

	virtual SymbolPtr visit(FunctionSymbolPtr function, ScopeState state) {
		for (auto it = function->parameters.begin();
		     it != function->parameters.end();
		     ++it) {
			acceptOn(it->type, state);

			if (it->symbol)
				it->symbol = assumeIsA<ParameterSymbol>(
					accept(it->symbol, state));
		}

		acceptOn(function->returnType, state.withScope(function->scope.get()));

		return function;
	}

	virtual SymbolPtr visit(DelayedSymbol delayed, ScopeState state) {
		SymbolPtr symbol = state.scope->lookup(delayed.name);
		
		if (!symbol) {
			context.diag.error(delayed.astNode.location(),
				"symbol not found: %s",
				delayed.name.c_str());
		}

		return symbol;
	}
};

class SemantecExpressionVisitor : public SemanticVisitorBase<ExpressionPtr> {
private:
	SemantecExpressionVisitor(Context& context)
		: SemanticVisitorBase<ExpressionPtr>(context) {}
	friend SemanticVisitors* makeSemanticVisitors(Context&);

protected:
	//ExpressionPtr visit(BlockExpressionPtr
};

#undef ID_VISIT


SemanticVisitors* makeSemanticVisitors(Context& context) {
	SemanticTypeVisitor* typeVisitor = new SemanticTypeVisitor(context);
	SemanticSymbolVisitor* symbolVisitor = new SemanticSymbolVisitor(context);
	SemantecExpressionVisitor* expressionVisitor =
		new SemantecExpressionVisitor(context);

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

