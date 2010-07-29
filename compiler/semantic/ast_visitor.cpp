#include "semantic/ast_visitor.hpp"

namespace llang {
namespace semantic {

class TypeVisitor : public ast::Visitor<ScopeState, Type*> {
private:
	AstVisitors* visitors;
	TypeVisitor() {}
	friend AstVisitors* makeAstVisitors();
};

class DeclarationVisitor : public ast::Visitor<ScopeState, Symbol*> {
private:
	AstVisitors* visitors;
	DeclarationVisitor() {}
	friend AstVisitors* makeAstVisitors();

protected:
	Symbol* visit(ast::Module& module, ScopeState state) {
		Module* symbol = new Module(module);

		for (auto it = module.declarations.begin();
		     it != module.declarations.end();
			 ++it) {
			
		}
	}

	Symbol* visit(ast::VariableDeclaration& variable, ScopeState state) {

	}

	Symbol* visit(ast::FunctionDeclaration& variable, ScopeState state) {

	}
};

class ExpressionVisitor : public ast::Visitor<ScopeState, Expression*> {
private:
	AstVisitors* visitors;
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
