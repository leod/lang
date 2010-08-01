#include <cassert>
#include <cstdio>

#include "lexer/token.hpp"
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

	template<typename T> void acceptOn(shared_ptr<T>& n, const ScopeState& p) {
		n = accept(n, p);
	}

	void acceptScope(Scope* scope, ScopeState state) {
		state.scope = scope;

		for (auto it = scope->symbols.begin();
		     it != scope->symbols.end();
		     ++it) {
			acceptOn(it->second, state);
		}
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

	ID_VISIT(IntegralType)

#undef ID_VISIT

	virtual TypePtr visit(FunctionTypePtr type, ScopeState state) {
		for (auto it = type->parameterTypes.begin();
		     it != type->parameterTypes.end();
			 ++it) {
			acceptOn(*it, state);	
		}

		acceptOn(type->returnType, state);

		return type;
	}

	virtual TypePtr visit(ArrayTypePtr type, ScopeState state) {
		acceptOn(type->inner, state);
		return type;
	}
};

class SemanticSymbolVisitor : public SemanticVisitorBase<SymbolPtr> {
private:
	SemanticSymbolVisitor(Context& context)
		: SemanticVisitorBase<SymbolPtr>(context) {}
	friend SemanticVisitors* makeSemanticVisitors(Context&);

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

	virtual SymbolPtr visit(ParameterSymbolPtr parameter, ScopeState state) {
		acceptOn(parameter->type, state);
		return parameter;
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

		acceptOn(function->returnType, state);
		acceptOn(function->body, state.withScope(function->scope.get()));

		// TODO
		if (!function->body->type->equals(function->returnType)) {
			context.diag.error(function->body->astNode.location(),
				"wrong type in function body expression of '%s': "
				"expected '%s', got '%s'",
				function->name.c_str(),
				function->returnType->name().c_str(),
				function->body->type->name().c_str());
		}

		return function;
	}

	virtual SymbolPtr visit(DelayedSymbolPtr delayed, ScopeState state) {
		SymbolPtr symbol = state.scope->lookup(delayed->name);
		
		if (!symbol) {
			context.diag.error(delayed->astNode.location(),
				"symbol not found: %s",
				delayed->name.c_str());
		}

		return symbol;
	}
};

class SemanticExpressionVisitor : public SemanticVisitorBase<ExpressionPtr> {
private:
	SemanticExpressionVisitor(Context& context)
		: SemanticVisitorBase<ExpressionPtr>(context) {}
	friend SemanticVisitors* makeSemanticVisitors(Context&);

protected:
#define ID_VISIT(type) \
	virtual ExpressionPtr visit(type##Ptr ptr, ScopeState) \
	{ return ptr; }

	ID_VISIT(VoidExpression)
	ID_VISIT(LiteralNumberExpression)
	ID_VISIT(LiteralStringExpression)

#undef ID_VISIT

	virtual ExpressionPtr visit(DelayedExpressionPtr delayed,
	                            ScopeState state) {
		SymbolPtr symbol = accept(delayed->delayedSymbol, state);
		
		TypePtr type;
		if (FunctionSymbolPtr function = isA<FunctionSymbol>(symbol))
			type = function->type;
		else if (VariableSymbolPtr variable = isA<VariableSymbol>(symbol)) {
			type = variable->type;
		}
		else if (ParameterSymbolPtr parameter = isA<ParameterSymbol>(symbol))
			type = parameter->type;

		return ExpressionPtr(new SymbolExpression(delayed->astNode, type,
		                                          symbol));
	}	

	virtual ExpressionPtr visit(BlockExpressionPtr block, ScopeState state) {
		acceptScope(block->scope.get(), state);
		state.scope = block->scope.get();

		for (auto it = block->expressions.begin();
             it != block->expressions.end();
             ++it) {
			acceptOn(*it, state);		
		}

		block->type = block->expressions.size() ?
			block->expressions.back()->type :
			TypePtr(new IntegralType(block->astNode,
			                         ast::IntegralType::VOID));

		return block;
	}

	virtual ExpressionPtr visit(CallExpressionPtr call, ScopeState state) {
		ExpressionPtr callee = call->callee = accept(call->callee, state);

		FunctionTypePtr type = isA<FunctionType>(callee->type);

		if (!type) { 
			std::string typeName = callee->type->name();
			context.diag.error(call->astNode.location(),
				"can call only functions, not '%s'", typeName.c_str());
		}

		if (type->parameterTypes.size() != call->arguments.size())
			context.diag.error(call->astNode.location(),
				"wrong number of parameters: expected %d, got %d",
				type->parameterTypes.size(),
				call->arguments.size());

		{
			size_t i = 0;
			auto it1 = call->arguments.begin();
			auto it2 = type->parameterTypes.begin();

			for (; it1 != call->arguments.end(); ++it1, ++it2, ++i) {
				ExpressionPtr argument = *it1 = accept(*it1, state); 

				if (!argument->type->equals(*it2)) {
					std::string expectedType = (*it2)->name();
					std::string gotType = argument->type->name();

					context.diag.error(call->astNode.location(),
						"wrong type in %d. argument of function call: "
						"expected '%s', got '%s'",
						i, expectedType.c_str(), gotType.c_str());
				}
			}
		}
		
		call->type = type->returnType;
		return call;
	}

	virtual ExpressionPtr visit(BinaryExpressionPtr binary, ScopeState state) {
		acceptOn(binary->left, state);
		acceptOn(binary->right, state);

		// TODO
		if (!binary->left->type->equals(binary->right->type))
			context.diag.error(binary->astNode.location(),
				"'%s' and '%s' are not the same type in binary expression",
				binary->left->type->name().c_str(),
				binary->right->type->name().c_str());

		binary->type = binary->left->type;

		return binary;
	}

	virtual ExpressionPtr visit(IfElseExpressionPtr ifElse, ScopeState state) {
		acceptOn(ifElse->condition, state);
		acceptOn(ifElse->ifExpression, state);
		acceptOn(ifElse->elseExpression, state);

		// TODO
		if (!ifElse->ifExpression->type->equals(ifElse->elseExpression->type))
			context.diag.error(ifElse->astNode.location(),
				"type of if and else expression need to be equivalent: "
				"'%s' vs '%s'",
				ifElse->ifExpression->type->name().c_str(),
				ifElse->elseExpression->type->name().c_str());

		ifElse->type = ifElse->ifExpression->type;

		return ifElse;
	}
};

SemanticVisitors* makeSemanticVisitors(Context& context) {
	SemanticTypeVisitor* typeVisitor = new SemanticTypeVisitor(context);
	SemanticSymbolVisitor* symbolVisitor = new SemanticSymbolVisitor(context);
	SemanticExpressionVisitor* expressionVisitor =
		new SemanticExpressionVisitor(context);

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

