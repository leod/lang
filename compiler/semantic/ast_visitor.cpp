#include <cassert>
#include <boost/make_shared.hpp>

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
		TypePtr type(accept(*variable.type, state));
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
			TypePtr type(accept(*it->type, state));

			ParameterSymbol* symbol = 0;
			if (it->hasName) {
				symbol = new ParameterSymbol(function, // TMP
				                             it->name, // TMP
				                             scope,
				                             type); 
				state.scope->addSymbol(symbol);
			}

			parameterTypes.push_back(type);
			parameters.push_back(FunctionSymbol::Parameter(type, symbol));
		}

		TypePtr returnType(accept(*function.returnType, state));
		Expression* body = accept(*function.body, state);

		TypePtr type(new FunctionType(function, returnType, parameterTypes));

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

		return new BinaryExpression(expression, left->type, left, right);
	}

	virtual Expression* visit(ast::IdentifierExpression& expression,
	                          ScopeState state) {
		// TODO: will probably need a second pass to handle forward refs

		Symbol* symbol = state.scope->lookup(expression.name);

		if (!symbol)
			context.diag.error(expression.location,
			                   "cannot find symbol '%s'",
			                   expression.name.c_str());

		// TODO: this isn't very good... maybe add a 'Type*' in the
		//       symbol base class?
		TypePtr type;
		if (FunctionSymbol* function = symbol->isA<FunctionSymbol>())
			type = function->type;
		else if (VariableSymbol* variable = symbol->isA<VariableSymbol>())
			type = variable->type;

		return new SymbolExpression(expression, type, symbol);
	}

	virtual Expression* visit(ast::CallExpression& call,
	                          ScopeState state) {
		Expression* callee = accept(*call.callee, state);

		FunctionType* type = callee->type->isA<FunctionType>();
		if (!type) 
			context.diag.error(call.location, "can call only functions");

		CallExpression::argument_list_t arguments;
		for (auto it = call.arguments.begin();
		     it != call.arguments.end();
		     ++it) {
			arguments.push_back(ExpressionPtr(accept(**it, state)));		
		}

		return new CallExpression(call, type->returnType, callee, arguments);
	}

	virtual Expression* visit(ast::BlockExpression& block,
	                          ScopeState state) {
		BlockExpression::expression_list_t expressions;
		for (auto it = block.expressions.begin();
		     it != block.expressions.end();
		     ++it) {
			expressions.push_back(ExpressionPtr(accept(**it, state)));
		}

		return new BlockExpression(block, expressions.back()->type,
		                           expressions);
	}

	virtual Expression* visit(ast::LiteralNumberExpression literal,
	                          ScopeState) {
		// TODO: This type is not right. Literal numbers have an unspecified
		//       int type and are implicitly casteable to i32.
		TypePtr type(new IntegralType(literal, lexer::Token::KEYWORD_I32));

		return new LiteralNumberExpression(literal, type);
	}

	virtual Expression* visit(ast::VoidExpression voidExpression, ScopeState) {
		TypePtr type(new IntegralType(voidExpression,
		                              lexer::Token::KEYWORD_VOID));
		return new VoidExpression(voidExpression, type);
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
