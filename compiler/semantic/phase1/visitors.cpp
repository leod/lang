#include <cassert>
#include <iostream>

#include "ast/type.hpp"
#include "ast/decl.hpp"
#include "ast/expr.hpp"
#include "semantic/phase1/visitors.hpp"

namespace llang {
namespace semantic {

using namespace ast;

namespace {

class TypeVisitor : public VisitorBase<TypePtr> {
private:
	TypeVisitor(Context& context)
		: VisitorBase<TypePtr>(context) {}
	friend Visitors* semantic::makePhase1Visitors(Context&);

protected:
	virtual TypePtr visit(ArrayTypePtr type, ScopeState state) {
		acceptOn(type->inner, state);
		return type;
	}

	virtual TypePtr visit(FunctionTypePtr type, ScopeState state) {
		acceptOn(type->returnType, state);
		acceptOn(type->parameterTypes.begin(), type->parameterTypes.end(),
		         state);
		return type;
	}

	virtual TypePtr visit(IntegralTypePtr type, ScopeState) {
		if (type->type == IntegralType::STRING) {
			TypePtr inner(new IntegralType(type->location(),
			                               IntegralType::CHAR));
			return TypePtr(new ArrayType(type->location(), inner));
		}
		
		return type;
	}
};

class DeclVisitor : public VisitorBase<DeclPtr> {
private:
	DeclVisitor(Context& context)
		: VisitorBase<DeclPtr>(context) {}
	friend Visitors* semantic::makePhase1Visitors(Context&);

protected:
	virtual DeclPtr visit(ModulePtr module, ScopeState state) {
		module->scope = ScopePtr(new Scope(0));
		state.scope = module->scope.get();

		for (auto it = module->decls.begin(); it != module->decls.end(); ++it) {
			acceptOn(*it, state);
			module->scope->addDecl(*it);
		}

		return module;
	}

	virtual DeclPtr visit(VariableDeclPtr variable, ScopeState state) {
		acceptOn(variable->type, state);
		acceptOn(variable->initializer, state);
		variable->declScope = state.scope;
		return variable;
	}

	virtual DeclPtr visit(ParameterDeclPtr variable, ScopeState state) {
		acceptOn(variable->type, state);
		variable->declScope = state.scope;
		return variable;
	}

	virtual DeclPtr visit(FunctionDeclPtr function, ScopeState state) {
		function->scope = ScopePtr(new Scope(state.scope));
		function->declScope = state.scope;
		state.scope = function->scope.get();

		FunctionType::ParameterTypeList parameterTypes;

		for (auto it = function->parameters.begin();
		     it != function->parameters.end();
		     ++it) {
			*it = assumeIsA<ParameterDecl>(accept(*it, state));

			if((*it)->hasName)
				state.scope->addDecl(*it);

			parameterTypes.push_back((*it)->type);
		}

		acceptOn(function->returnType, state);
		if (function->body) acceptOn(function->body, state);

		function->type = TypePtr(new FunctionType(function->location(),
		                                          function->returnType,
		                                          parameterTypes));

		return function;
	}
};

class ExprVisitor : public VisitorBase<ExprPtr> {
private:
	ExprVisitor(Context& context)
		: VisitorBase<ExprPtr>(context) {}
	friend Visitors* semantic::makePhase1Visitors(Context&);

protected:
	virtual ExprPtr visit(BinaryExprPtr expr, ScopeState state) {
		acceptOn(expr->left, state);
		acceptOn(expr->right, state);

		return expr;
	}

	virtual ExprPtr visit(IdentifierExprPtr identifier, ScopeState state) {
		// Delay looking up the decl to the next semantic phase

		DeclPtr decl(
			new DelayedDecl(identifier->location(), identifier->name));
		decl->declScope = state.scope;

		ExprPtr expr(
			new DelayedExpr(identifier->location(), decl));
     
		return expr;
	}

	virtual ExprPtr visit(CallExprPtr call, ScopeState state) {
		acceptOn(call->callee, state);
		acceptOn(call->arguments.begin(), call->arguments.end(), state);

		return call;
	}

	virtual ExprPtr visit(BlockExprPtr block, ScopeState state) {
		block->scope = ScopePtr(new Scope(state.scope));

		acceptOn(block->exprs.begin(), block->exprs.end(),
		         state.withScope(block->scope.get()));

		return block;
	}

	virtual ExprPtr visit(LiteralNumberExprPtr literal, ScopeState) {
		//TypePtr type(new NumberType(literal->location()));

		// TODO: hardcoded type
		TypePtr type(new IntegralType(literal->location(),
		                              IntegralType::I32));

		literal->type = type;

		return literal;
	}

	virtual ExprPtr visit(LiteralStringExprPtr literal, ScopeState) {
		TypePtr type(new ArrayType(literal->location(),
			TypePtr(new IntegralType(literal->location(),
			                         IntegralType::CHAR))));
		
		literal->type = type;

		return literal;
	}

	virtual ExprPtr visit(LiteralBoolExprPtr literal, ScopeState) {
		TypePtr type(new IntegralType(literal->location(), IntegralType::BOOL));

		literal->type = type;

		return literal;
	}

	virtual ExprPtr visit(VoidExprPtr voidExpr, ScopeState) {
		TypePtr type(new IntegralType(voidExpr->location(),
		                              IntegralType::VOID));
		
		voidExpr->type = type;

		return voidExpr;
	}

	virtual ExprPtr visit(DeclExprPtr declExpr, ScopeState state) {
		DeclPtr decl = accept(declExpr->decl, state); 

		state.scope->addDecl(decl);
		declExpr->decl = decl;

		TypePtr type(new IntegralType(declExpr->location(), IntegralType::VOID));
		declExpr->type = type;

		return declExpr;
	}	

	virtual ExprPtr visit(IfElseExprPtr ifElse, ScopeState state) {
		acceptOn(ifElse->condition, state);
		acceptOn(ifElse->ifExpr, state);
		acceptOn(ifElse->elseExpr, state);

		return ifElse;
	}

	virtual ExprPtr visit(ArrayElementExprPtr element, ScopeState state) {
		acceptOn(element->array, state);
		acceptOn(element->index, state);

		return element;
	}
};

} // namespace

Visitors* makePhase1Visitors(Context& context) {
	TypeVisitor* typeVisitor = new TypeVisitor(context);
	DeclVisitor* declVisitor = new DeclVisitor(context);
	ExprVisitor* exprVisitor = new ExprVisitor(context);

	Visitors* visitors = new Visitors(typeVisitor, declVisitor, exprVisitor);
	
	typeVisitor->visitors = visitors;
	declVisitor->visitors = visitors;
	exprVisitor->visitors = visitors;

	return visitors;
}

} // namespace semantic
} // namespace llang
