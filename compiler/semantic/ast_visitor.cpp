#include <cassert>

#include "common/context.hpp"
#include "semantic/ast_visitor.hpp"

namespace llang {
namespace semantic {

template <typename Param, typename Result> class VisitorBase
	: public ast::Visitor<Param, Result> {
protected:
	AstVisitors* visitors;
	Context& context;

	VisitorBase(Context& context) : context(context) {}
	friend AstVisitors* makeAstVisitors(Context&);

	Type* accept(ast::Type& n, const Param& p) {
		return visitors->typeVisitor->accept(n, p);
	}

	Symbol* accept(ast::Declaration& n, const Param& p) {
		return visitors->declarationVisitor->accept(n, p);
	}

	Expression* accept(ast::Expression& n, const Param& p) {
		return visitors->expressionVisitor->accept(n, p);
	}
};

class TypeVisitor : public VisitorBase<ScopeState, Type*> {
private:
	TypeVisitor(Context& context)
		: VisitorBase<ScopeState, Type*>(context) {}
	friend AstVisitors* makeAstVisitors(Context&);

protected:
	virtual Type* visit(ast::IntegralType& type, ScopeState) {
		return new IntegralType(type);				
	}
};

class DeclarationVisitor : public VisitorBase<ScopeState, Symbol*> {
private:
	DeclarationVisitor(Context& context)
		: VisitorBase<ScopeState, Symbol*>(context) {}
	friend AstVisitors* makeAstVisitors(Context&);

protected:
	virtual Symbol* visit(ast::Module& module, ScopeState state) {
		Module* symbol = new Module(module);

		state.scope = symbol->scope;

		for (auto it = module.declarations.begin();
		     it != module.declarations.end();
			 ++it) {
			symbol->scope->addSymbol(accept(**it, state));
		}

		return symbol;
	}

	virtual Symbol* visit(ast::VariableDeclaration& variable,
	                      ScopeState state) {
		Type* type = accept(*variable.type, state);
		Expression* initializer = accept(*variable.initializer, state);
		
		VariableSymbol* symbol = new VariableSymbol(variable, state.scope, type,
		                                            initializer);
		state.scope->addSymbol(symbol);

		return symbol;
	}

	virtual Symbol* visit(ast::FunctionDeclaration& function,
	                      ScopeState state) {
		Scope* scope = new Scope(state.scope);
		state.scope = scope;

		FunctionType::parameter_type_list_t parameterTypes;
		FunctionSymbol::parameter_list_t parameters;

		for (auto it = function.parameters.begin();
		     it != function.parameters.end();
			 ++it) {
			Type* type = accept(*it->type, state);

			ParameterSymbol* symbol = 0;
			if (it->hasName) {
				symbol = new ParameterSymbol(function, // TMP
				                             it->name, // TMP
				                             scope,
				                             type); 
				state.scope->addSymbol(symbol);
			}

			parameterTypes.push_back(TypePtr(type));
			parameters.push_back(FunctionSymbol::Parameter(type, symbol));
		}

		Type* returnType = accept(*function.returnType, state);
		Expression* body = accept(*function.body, state);

		FunctionType* type = new FunctionType(function, returnType,
		                                      parameterTypes);

		FunctionSymbol* symbol = new FunctionSymbol(function, scope, returnType,
		                                            parameters, body, type);

		return symbol;
	}
};

class ExpressionVisitor : public VisitorBase<ScopeState, Expression*> {
private:
	ExpressionVisitor(Context& context)
		: VisitorBase<ScopeState, Expression*>(context) {}
	friend AstVisitors* makeAstVisitors(Context&);

protected:
	virtual Expression*	visit(ast::BinaryExpression& expression,
	                          ScopeState state) {
		Expression* left = accept(*expression.left, state);
		Expression* right = accept(*expression.right, state);

		// TODO: later we'll want to do implicit casts here, probably
		if (!left->type->equals(right->type.get())) assert(false);

		return new BinaryExpression(expression, left->type.get(), left, right);
	}

	virtual Expression* visit(ast::IdentifierExpression& expression,
	                          ScopeState state) {
		
	}
};

AstVisitors* makeAstVisitors(Context& context) {
	TypeVisitor* typeVisitor = new TypeVisitor(context);
	DeclarationVisitor* declarationVisitor = new DeclarationVisitor(context);
	ExpressionVisitor* expressionVisitor = new ExpressionVisitor(context);

	AstVisitors* visitors = new AstVisitors(typeVisitor, declarationVisitor,
	                                        expressionVisitor);
	
	typeVisitor->visitors = visitors;
	declarationVisitor->visitors = visitors;
	typeVisitor->visitors = visitors;

	return visitors;
}

} // namespace ast
} // namespace llang
