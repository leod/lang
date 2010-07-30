#include <cassert>
#include <boost/make_shared.hpp>

#include "ast/print_visitor.hpp"
#include "semantic/ast_visitor.hpp"
#include "semantic/symbol.hpp"
#include "semantic/expression.hpp"
#include "semantic/type.hpp"

namespace llang {
namespace semantic {

template <typename Result> class VisitorBase
	: public ast::Visitor<ScopeState, Result> {
protected:
	AstVisitors* visitors;
	Context& context;

	VisitorBase(Context& context) : context(context) {}
	friend AstVisitors* makeAstVisitors(Context&);

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
	friend AstVisitors* makeAstVisitors(Context&);

protected:
	virtual TypePtr visit(ast::IntegralType& type, ScopeState) {
		return TypePtr(new IntegralType(type));
	}
};

class DeclarationVisitor : public VisitorBase<SymbolPtr> {
private:
	DeclarationVisitor(Context& context)
		: VisitorBase<SymbolPtr>(context) {}
	friend AstVisitors* makeAstVisitors(Context&);

protected:
	virtual SymbolPtr visit(ast::Module& module, ScopeState state) {
		Scope* scope = new Scope(0);
		state.scope = scope;

		boost::shared_ptr<Module> symbol(new Module(module, scope));

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

		if (!initializer->type->equals(type)) {
			std::string expectedType = type->name();
			std::string gotType = initializer->type->name();

			context.diag.error(variable.location,
				"wrong type in initializer of %s: expected '%s', got '%s'",
				variable.name.c_str(), expectedType.c_str(), gotType.c_str());
		}
		
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

class ExpressionVisitor : public VisitorBase<ExpressionPtr> {
private:
	ExpressionVisitor(Context& context)
		: VisitorBase<ExpressionPtr>(context) {}
	friend AstVisitors* makeAstVisitors(Context&);

protected:
	virtual ExpressionPtr visit(ast::BinaryExpression& expression,
	                            ScopeState state) {
		ExpressionPtr left = accept(*expression.left, state);
		ExpressionPtr right = accept(*expression.right, state);

		// TODO: later we'll want to do implicit casts here, probably
		if (!left->type->equals(right->type.get())) assert(false);

		return ExpressionPtr(new BinaryExpression(expression, left->type,
		                                          left, right));
	}

	virtual ExpressionPtr visit(ast::IdentifierExpression& expression,
	                            ScopeState state) {
		// TODO: will probably need a second pass to handle forward refs

		SymbolPtr symbol = state.scope->lookup(expression.name);

		if (!symbol)
			context.diag.error(expression.location,
			                   "cannot find symbol '%s'",
			                   expression.name.c_str());

		// TODO: this isn't very good... maybe add a TypePtr in the
		//       symbol base class?
		TypePtr type;
		if (boost::shared_ptr<FunctionSymbol> function =
			isA<FunctionSymbol>(symbol))
			type = function->type;
		else if (boost::shared_ptr<VariableSymbol> variable =
			isA<VariableSymbol>(symbol))
			type = variable->type;
		else if (boost::shared_ptr<ParameterSymbol> parameter =
			isA<ParameterSymbol>(symbol))
			type = parameter->type;
		else
			context.diag.error(expression.location,
			                   "cannot use %s in expression",
			                   expression.name.c_str());

		return ExpressionPtr(new SymbolExpression(expression, type, symbol));
	}

	virtual ExpressionPtr visit(ast::CallExpression& call,
	                            ScopeState state) {
		ExpressionPtr callee = accept(*call.callee, state);

		boost::shared_ptr<FunctionType> type = isA<FunctionType>(callee->type);

		if (!type) { 
			std::string typeName = callee->type->name();
			context.diag.error(call.location,
				"can call only functions, not '%s'", typeName.c_str());
		}

		if (type->parameterTypes.size() != call.arguments.size())
			context.diag.error(call.location,
				"wrong number of parameters: expected %d, got %d",
				type->parameterTypes.size(),
				call.arguments.size());

		CallExpression::argument_list_t arguments;

		{
			size_t i = 0;
			auto it1 = call.arguments.begin();
			auto it2 = type->parameterTypes.begin();

			for (; it1 != call.arguments.end(); ++it1, ++it2, ++i) {
				ExpressionPtr argument(accept(**it1, state));

				if (!argument->type->equals(*it2)) {
					std::string expectedType = (*it2)->name();
					std::string gotType = argument->type->name();

					context.diag.error(call.location,
						"wrong type in %d. argument of function call:"
						"expected '%s', got '%s'",
						i, expectedType.c_str(), gotType.c_str());
				}

				arguments.push_back(argument);		
			}
		}

		return ExpressionPtr(
			new CallExpression(call, type->returnType, callee, arguments));
	}

	virtual ExpressionPtr visit(ast::BlockExpression& block,
	                            ScopeState state) {
		BlockExpression::expression_list_t expressions;
		for (auto it = block.expressions.begin();
		     it != block.expressions.end();
		     ++it) {
			expressions.push_back(accept(**it, state));
		}

		TypePtr type = expressions.size() ? expressions.back()->type
			: TypePtr(new IntegralType(block, lexer::Token::KEYWORD_VOID));

		return ExpressionPtr(new BlockExpression(block, type, expressions));
	}

	virtual ExpressionPtr visit(ast::LiteralNumberExpression& literal,
	                            ScopeState) {
		// TODO: This type is not right. Literal numbers have an unspecified
		//       int type and are implicitly casteable to i32.
		TypePtr type(new IntegralType(literal, lexer::Token::KEYWORD_I32));

		return ExpressionPtr(new LiteralNumberExpression(literal, type));
	}

	virtual ExpressionPtr visit(ast::VoidExpression& voidExpression, ScopeState) {
		TypePtr type(new IntegralType(voidExpression,
		                              lexer::Token::KEYWORD_VOID));
		return ExpressionPtr(new VoidExpression(voidExpression, type));
	}

	virtual ExpressionPtr visit(ast::DeclarationExpression& declaration,
                              ScopeState state) {
		state.scope->addSymbol(accept(*declaration.declaration, state));

		TypePtr type(new IntegralType(declaration,
		                              lexer::Token::KEYWORD_VOID));
		return ExpressionPtr(new VoidExpression(declaration, type));
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
	expressionVisitor->visitors = visitors;

	return visitors;
}

} // namespace ast
} // namespace llang
