#include <cassert>
#include <cstdio>

#include "ast/decl.hpp"
#include "ast/expr.hpp"
#include "ast/type.hpp"
#include "ast/type_test.hpp"
#include "semantic/phase2/visitors.hpp"

namespace llang {
namespace semantic {

using namespace ast;

namespace {

class TypeVisitor : public VisitorBase<TypePtr> {
private:
	TypeVisitor(Context& context)
		: VisitorBase<TypePtr>(context) {}
	friend Visitors* semantic::makePhase2Visitors(Context&);

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

class DeclVisitor : public VisitorBase<DeclPtr> {
private:
	DeclVisitor(Context& context)
		: VisitorBase<DeclPtr>(context) {}
	friend Visitors* semantic::makePhase2Visitors(Context&);

protected:
	virtual DeclPtr visit(ModulePtr module, ScopeState state) {
		acceptScope(module->scope.get(), state);
		return module;
	}

	virtual DeclPtr visit(VariableDeclPtr variable, ScopeState state) {
		acceptOn(variable->type, state);
		acceptOn(variable->initializer, state);

		if (!variable->type->equals(variable->initializer->type)) {
			context.diag.error(variable->location(),
				"initializer of %s has wrong type: "
				"expected '%s', got '%s'",
				variable->name.c_str(),
				variable->type->name().c_str(),
				variable->initializer->type->name().c_str());
		}

		return variable;
	}

	virtual DeclPtr visit(ParameterDeclPtr parameter, ScopeState state) {
		acceptOn(parameter->type, state);
		return parameter;
	}

	virtual DeclPtr visit(FunctionDeclPtr function, ScopeState state) {
		for (auto it = function->parameters.begin();
		     it != function->parameters.end();
		     ++it) {
			DeclPtr decl = *it;
			acceptOn(decl, state);
			*it = assumeIsA<ParameterDecl>(decl);
		}

		acceptOn(function->returnType, state);
		acceptOn(function->body, state.withScope(function->scope.get()));

		// TODO
		if (!function->body->type->equals(function->returnType)) {
			context.diag.error(function->body->location(),
				"wrong type in function body expr of '%s': "
				"expected '%s', got '%s'",
				function->name.c_str(),
				function->returnType->name().c_str(),
				function->body->type->name().c_str());
		}

		return function;
	}

	virtual DeclPtr visit(DelayedDeclPtr delayed, ScopeState state) {
		DeclPtr decl = state.scope->lookup(delayed->name);
		
		if (!decl) {
			context.diag.error(delayed->location(),
				"symbol not found: %s",
				delayed->name.c_str());
		}

		return decl;
	}
};

class ExprVisitor : public VisitorBase<ExprPtr> {
private:
	ExprVisitor(Context& context)
		: VisitorBase<ExprPtr>(context) {}
	friend Visitors* semantic::makePhase2Visitors(Context&);

protected:
#define ID_VISIT(type) \
	virtual ExprPtr visit(type##Ptr ptr, ScopeState) \
	{ return ptr; }

	ID_VISIT(VoidExpr)
	ID_VISIT(LiteralNumberExpr)
	ID_VISIT(LiteralStringExpr)
	ID_VISIT(LiteralBoolExpr)
	ID_VISIT(DeclExpr)

#undef ID_VISIT

	virtual ExprPtr visit(DelayedExprPtr delayed, ScopeState state) {
		DeclPtr decl = accept(delayed->delayedDecl, state);
		
		TypePtr type;
		if (FunctionDeclPtr function = isA<FunctionDecl>(decl))
			type = function->type;
		else if (VariableDeclPtr variable = isA<VariableDecl>(decl))
			type = variable->type;
		else if (ParameterDeclPtr parameter = isA<ParameterDecl>(decl))
			type = parameter->type;

		assert(type);

		return ExprPtr(new DeclRefExpr(delayed->location(), type, decl));
	}	

	virtual ExprPtr visit(BlockExprPtr block, ScopeState state) {
		acceptScope(block->scope.get(), state);
		state.scope = block->scope.get();

		for (auto it = block->exprs.begin();
             it != block->exprs.end();
             ++it) {
			acceptOn(*it, state);		
		}

		block->type = block->exprs.size() ?
			block->exprs.back()->type :
			TypePtr(new IntegralType(block->location(),
			                         ast::IntegralType::VOID));

		return block;
	}

	virtual ExprPtr visit(CallExprPtr call, ScopeState state) {
		acceptOn(call->callee, state);
		ExprPtr callee = call->callee;

		FunctionTypePtr type = isA<FunctionType>(callee->type);

		if (!type) { 
			std::string typeName = callee->type->name();
			context.diag.error(call->location(),
				"can call only functions, not '%s'", typeName.c_str());
		}

		if (type->parameterTypes.size() != call->arguments.size())
			context.diag.error(call->location(),
				"wrong number of parameters: expected %d, got %d",
				type->parameterTypes.size(),
				call->arguments.size());

		{
			size_t i = 0;
			auto it1 = call->arguments.begin();
			auto it2 = type->parameterTypes.begin();

			for (; it1 != call->arguments.end(); ++it1, ++it2, ++i) {
				ExprPtr argument = *it1 = accept(*it1, state); 

				if (!argument->type->equals(*it2)) {
					std::string expectedType = (*it2)->name();
					std::string gotType = argument->type->name();

					context.diag.error(call->location(),
						"wrong type in %d. argument of function call: "
						"expected '%s', got '%s'",
						i, expectedType.c_str(), gotType.c_str());
				}
			}
		}
		
		call->type = type->returnType;
		return call;
	}

	virtual ExprPtr visit(BinaryExprPtr binary, ScopeState state) {
		acceptOn(binary->left, state);
		acceptOn(binary->right, state);

		// TODO
		if (!binary->left->type->equals(binary->right->type))
			context.diag.error(binary->location(),
				"'%s' and '%s' are not the same type in binary expr",
				binary->left->type->name().c_str(),
				binary->right->type->name().c_str());

		if (binary->operation == ast::BinaryExpr::EQUALS) {
			binary->type = TypePtr(new IntegralType(binary->location(),
			                                        ast::IntegralType::BOOL));	
		}
		else
			binary->type = binary->left->type;

		return binary;
	}

	virtual ExprPtr visit(IfElseExprPtr ifElse, ScopeState state) {
		acceptOn(ifElse->condition, state);
		acceptOn(ifElse->ifExpr, state);
		acceptOn(ifElse->elseExpr, state);

		if (!isBool(ifElse->condition->type))
			context.diag.error(ifElse->location(),
				"if condition needs to be boolean (got '%s')",
				ifElse->condition->type->name().c_str());

		// TODO
		if (!ifElse->ifExpr->type->equals(ifElse->elseExpr->type))
			context.diag.error(ifElse->location(),
				"type of if and else expr need to be equivalent: "
				"'%s' vs '%s'",
				ifElse->ifExpr->type->name().c_str(),
				ifElse->elseExpr->type->name().c_str());

		ifElse->type = ifElse->ifExpr->type;

		return ifElse;
	}
};

} // namespace

Visitors* makePhase2Visitors(Context& context) {
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

