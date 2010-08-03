#include <cassert>

#include "ast/print_visitor.hpp"
#include "ast/type.hpp"
#include "ast/declaration.hpp"
#include "ast/expression.hpp"
#include "semantic/ast_visitor.hpp"
#include "semantic/symbol.hpp"
#include "semantic/expression.hpp"
#include "semantic/type.hpp"

namespace llang {
namespace semantic {

namespace {

template <typename Result> class VisitorBase
	: public ast::Visitor<ScopeState, Result> {
protected:
	AstVisitors* visitors;
	Context& context;

	VisitorBase(Context& context) : context(context) {}

	TypePtr accept(ast::Type& n, const ScopeState& p) {
		return visitors->typeVisitor->accept(n, p);
	}

	SymbolPtr accept(ast::Declaration& n, const ScopeState& p) {
		return visitors->declarationVisitor->accept(n, p);
	}

	ExpressionPtr accept(ast::Expression& n, const ScopeState& p) {
		return visitors->expressionVisitor->accept(n, p);
	}
};

class TypeVisitor : public VisitorBase<TypePtr> {
private:
	TypeVisitor(Context& context)
		: VisitorBase<TypePtr>(context) {}
	friend AstVisitors* semantic::makeAstVisitors(Context&);

protected:
	virtual TypePtr visit(ast::IntegralType& type, ScopeState) {
		if (type.type == ast::IntegralType::STRING)
			return TypePtr(new ArrayType(type,
				TypePtr(new IntegralType(type, ast::IntegralType::CHAR))));
		
		return TypePtr(new IntegralType(type));
	}
};

class DeclarationVisitor : public VisitorBase<SymbolPtr> {
private:
	DeclarationVisitor(Context& context)
		: VisitorBase<SymbolPtr>(context) {}
	friend AstVisitors* semantic::makeAstVisitors(Context&);

protected:
	virtual SymbolPtr visit(ast::Module& module, ScopeState state) {
		Scope* scope = new Scope(0); // TODO
		state.scope = scope;

		shared_ptr<Module> symbol(new Module(module, scope));

		for (auto it = module.declarations.begin();
		     it != module.declarations.end();
		     ++it) {
			symbol->scope->addSymbol(accept(**it, state));
		}

		return symbol;
	}

	virtual SymbolPtr visit(ast::VariableDeclaration& variable,
	                        ScopeState state) {
		TypePtr type = accept(*variable.type, state);
		ExpressionPtr initializer = accept(*variable.initializer, state);

		SymbolPtr symbol(new VariableSymbol(variable, state.scope, type,
		                                    initializer));

		return symbol;
	}

	virtual SymbolPtr visit(ast::FunctionDeclaration& function,
	                        ScopeState state) {
		Scope* scope = new Scope(state.scope);
		state.scope = scope;

		FunctionType::parameter_type_list_t parameterTypes;
		FunctionSymbol::parameter_list_t parameters;

		for (auto it = function.parameters.begin();
		     it != function.parameters.end();
		     ++it) {
			TypePtr type(accept(*it->type, state));

			ParameterSymbolPtr symbol;
			if (it->hasName) {
				symbol.reset(new ParameterSymbol(function, // TMP
				                                 it->name, // TMP
				                                 scope,
				                                 type)); 
				state.scope->addSymbol(symbol);
			}

			parameterTypes.push_back(type);
			parameters.push_back(FunctionSymbol::Parameter(type, symbol));
		}

		TypePtr returnType(accept(*function.returnType, state));
		ExpressionPtr body = accept(*function.body, state);

		TypePtr type(new FunctionType(function, returnType, parameterTypes));

		SymbolPtr symbol(new FunctionSymbol(function, scope, returnType,
		                                    parameters, body, type));

		return symbol;
	}
};

// Does nothing more than translate the ast into the semantic tree.
// Typechecking is done in the second semantic phase.
// This should allow us to easily deal with forward references.
class ExpressionVisitor : public VisitorBase<ExpressionPtr> {
private:
	ExpressionVisitor(Context& context)
		: VisitorBase<ExpressionPtr>(context) {}
	friend AstVisitors* semantic::makeAstVisitors(Context&);

protected:
	virtual ExpressionPtr visit(ast::BinaryExpression& expression,
	                            ScopeState state) {
		ExpressionPtr left = accept(*expression.left, state);
		ExpressionPtr right = accept(*expression.right, state);

		return ExpressionPtr(new BinaryExpression(expression,
		                                          UndefinedType::singleton(),
		                                          left, right));
	}

	virtual ExpressionPtr visit(ast::IdentifierExpression& identifier,
	                            ScopeState) {
		// Delay looking up the symbol to the next semantic phase

		SymbolPtr symbol(new DelayedSymbol(identifier, identifier.name, 0));
		TypePtr type(new DelayedType(identifier, symbol));
		ExpressionPtr expression(
			new DelayedExpression(identifier, type, symbol));
     
		return expression;
	}

	virtual ExpressionPtr visit(ast::CallExpression& call,
	                            ScopeState state) {
		ExpressionPtr callee = accept(*call.callee, state);

		CallExpression::argument_list_t arguments;

		for (auto it = call.arguments.begin();
			 it != call.arguments.end();
			 ++it) {
			ExpressionPtr argument(accept(**it, state));
			arguments.push_back(argument);		
		}

		return ExpressionPtr(
			new CallExpression(call, UndefinedType::singleton(), callee,
			                   arguments));
	}

	virtual ExpressionPtr visit(ast::BlockExpression& block,
	                            ScopeState state) {
		Scope* scope = new Scope(state.scope); // TODO
		state.scope = scope;

		BlockExpression::expression_list_t expressions;
		for (auto it = block.expressions.begin();
		     it != block.expressions.end();
		     ++it) {
			expressions.push_back(accept(**it, state));
		}

		return ExpressionPtr(new BlockExpression(block,
		                                         UndefinedType::singleton(),
		                                         scope,
		                                         expressions));
	}

	virtual ExpressionPtr visit(ast::LiteralNumberExpression& literal,
	                            ScopeState) {
		// TODO: This type is not right. Literal numbers have an unspecified
		//       int type and are implicitly casteable to i32.
		
		TypePtr type(new IntegralType(literal, ast::IntegralType::I32));

		return ExpressionPtr(new LiteralNumberExpression(literal, type));
	}

	virtual ExpressionPtr visit(ast::LiteralStringExpression& literal,
	                            ScopeState) {
		TypePtr type(new ArrayType(literal,
				TypePtr(new IntegralType(literal, ast::IntegralType::CHAR))));

		return ExpressionPtr(new LiteralStringExpression(literal, type));
	}

	virtual ExpressionPtr visit(ast::VoidExpression& voidExpression,
	                            ScopeState) {
		TypePtr type(new IntegralType(voidExpression,
		                              ast::IntegralType::VOID));
		return ExpressionPtr(new VoidExpression(voidExpression, type));
	}

	virtual ExpressionPtr visit(ast::DeclarationExpression& declaration,
                                ScopeState state) {
		SymbolPtr symbol(accept(*declaration.declaration, state));
		state.scope->addSymbol(symbol);

		TypePtr type(new IntegralType(declaration, ast::IntegralType::VOID));
		return ExpressionPtr(
			new DeclarationExpression(declaration, type, symbol));
	}	

	virtual ExpressionPtr visit(ast::IfElseExpression& ifElse,
	                            ScopeState state) {
		return ExpressionPtr(
			new IfElseExpression(ifElse,
			                     UndefinedType::singleton(),
			                     accept(*ifElse.condition, state),
			                     accept(*ifElse.ifExpression, state),
			                     accept(*ifElse.elseExpression, state)));
	}
};

} // namespace

AstVisitors* makeAstVisitors(Context& context) {
	TypeVisitor* typeVisitor = new TypeVisitor(context);
	DeclarationVisitor* declarationVisitor =
		new DeclarationVisitor(context);
	ExpressionVisitor* expressionVisitor = new ExpressionVisitor(context);

	AstVisitors* visitors = new AstVisitors(typeVisitor, declarationVisitor,
	                                        expressionVisitor);
	
	typeVisitor->visitors = visitors;
	declarationVisitor->visitors = visitors;
	expressionVisitor->visitors = visitors;

	return visitors;
}

} // namespace semantic
} // namespace llang
