#include "semantic/ast_visitor.hpp"

namespace llang {
namespace semantic {

template <typename Param, typename Result> class VisitorBase
	: public ast::Visitor<Param, Result> {
protected:
	AstVisitors* visitors;
	VisitorBase() {}
	friend AstVisitors* makeAstVisitors();

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
	TypeVisitor() {}
	friend AstVisitors* makeAstVisitors();
};

class DeclarationVisitor : public VisitorBase<ScopeState, Symbol*> {
private:
	DeclarationVisitor() {}
	friend AstVisitors* makeAstVisitors();

protected:
	Symbol* visit(ast::Module& module, ScopeState state) {
		Module* symbol = new Module(module);

		state.scope = symbol->scope;

		for (auto it = module.declarations.begin();
		     it != module.declarations.end();
			 ++it) {
			symbol->scope->addSymbol(accept(**it, state));
		}

		return symbol;
	}

	Symbol* visit(ast::VariableDeclaration& variable, ScopeState state) {
		Type* type = accept(*variable.type, state);
		Expression* initializer = accept(*variable.initializer, state);
		
		VariableSymbol* symbol = new VariableSymbol(variable, state.scope, type,
		                                            initializer);
		state.scope->addSymbol(symbol);

		return symbol;
	}

	Symbol* visit(ast::FunctionDeclaration& function, ScopeState state) {
		FunctionSymbol::parameter_list_t parameters;

		for (auto it = function.parameters.begin();
		     it != function.parameters.end();
			 ++it) {
			ParameterSymbol* symbol = 0;
			if (it->hasName) {
				symbol = new ParameterSymbol(function, // TMP
				                             it->name, // TMP
				                             state.scope,
				                             accept(*it->type, state)); 
				state.scope->addSymbol(symbol);
			}
		}
	}
};

class ExpressionVisitor : public VisitorBase<ScopeState, Expression*> {
private:
	ExpressionVisitor() {}
	friend AstVisitors* makeAstVisitors();
};

AstVisitors* makeAstVisitors() {
	TypeVisitor* typeVisitor = new TypeVisitor;
	DeclarationVisitor* declarationVisitor = new DeclarationVisitor;
	ExpressionVisitor* expressionVisitor = new ExpressionVisitor;

	AstVisitors* visitors = new AstVisitors(typeVisitor, declarationVisitor,
	                                        expressionVisitor);
	
	typeVisitor->visitors = visitors;
	declarationVisitor->visitors = visitors;
	typeVisitor->visitors = visitors;

	return visitors;
}

} // namespace ast
} // namespace llang
