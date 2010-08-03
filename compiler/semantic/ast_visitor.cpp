#include <cassert>

#include "ast/print_visitor.hpp"
#include "ast/type.hpp"
#include "ast/decl.hpp"
#include "ast/expr.hpp"
#include "semantic/ast_visitor.hpp"
#include "semantic/decl.hpp"
#include "semantic/expr.hpp"
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

	DeclPtr accept(ast::Decl& n, const ScopeState& p) {
		return visitors->declVisitor->accept(n, p);
	}

	ExprPtr accept(ast::Expr& n, const ScopeState& p) {
		return visitors->exprVisitor->accept(n, p);
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

class DeclVisitor : public VisitorBase<DeclPtr> {
private:
	DeclVisitor(Context& context)
		: VisitorBase<DeclPtr>(context) {}
	friend AstVisitors* semantic::makeAstVisitors(Context&);

protected:
	virtual DeclPtr visit(ast::Module& module, ScopeState state) {
		Scope* scope = new Scope(0); // TODO
		state.scope = scope;

		shared_ptr<Module> decl(new Module(module, scope));

		for (auto it = module.decls.begin();
		     it != module.decls.end();
		     ++it) {
			decl->scope->addDecl(accept(**it, state));
		}

		return decl;
	}

	virtual DeclPtr visit(ast::VariableDecl& variable,
	                        ScopeState state) {
		TypePtr type = accept(*variable.type, state);
		ExprPtr initializer = accept(*variable.initializer, state);

		DeclPtr decl(new VariableDecl(variable, state.scope, type,
		                                    initializer));

		return decl;
	}

	virtual DeclPtr visit(ast::FunctionDecl& function,
	                        ScopeState state) {
		Scope* scope = new Scope(state.scope);
		state.scope = scope;

		FunctionType::parameter_type_list_t parameterTypes;
		FunctionDecl::parameter_list_t parameters;

		for (auto it = function.parameters.begin();
		     it != function.parameters.end();
		     ++it) {
			TypePtr type(accept(*it->type, state));

			ParameterDeclPtr decl;
			if (it->hasName) {
				decl.reset(new ParameterDecl(function, // TMP
				                                 it->name, // TMP
				                                 scope,
				                                 type)); 
				state.scope->addDecl(decl);
			}

			parameterTypes.push_back(type);
			parameters.push_back(FunctionDecl::Parameter(type, decl));
		}

		TypePtr returnType(accept(*function.returnType, state));
		ExprPtr body = accept(*function.body, state);

		TypePtr type(new FunctionType(function, returnType, parameterTypes));

		DeclPtr decl(new FunctionDecl(function, scope, returnType,
		                                    parameters, body, type));

		return decl;
	}
};

// Does nothing more than translate the ast into the semantic tree.
// Typechecking is done in the second semantic phase.
// This should allow us to easily deal with forward references.
class ExprVisitor : public VisitorBase<ExprPtr> {
private:
	ExprVisitor(Context& context)
		: VisitorBase<ExprPtr>(context) {}
	friend AstVisitors* semantic::makeAstVisitors(Context&);

protected:
	virtual ExprPtr visit(ast::BinaryExpr& expr,
	                            ScopeState state) {
		ExprPtr left = accept(*expr.left, state);
		ExprPtr right = accept(*expr.right, state);

		return ExprPtr(new BinaryExpr(expr,
		                                          UndefinedType::singleton(),
		                                          left, right));
	}

	virtual ExprPtr visit(ast::IdentifierExpr& identifier,
	                            ScopeState) {
		// Delay looking up the decl to the next semantic phase

		DeclPtr decl(new DelayedDecl(identifier, identifier.name, 0));
		TypePtr type(new DelayedType(identifier, decl));
		ExprPtr expr(
			new DelayedExpr(identifier, type, decl));
     
		return expr;
	}

	virtual ExprPtr visit(ast::CallExpr& call,
	                            ScopeState state) {
		ExprPtr callee = accept(*call.callee, state);

		CallExpr::argument_list_t arguments;

		for (auto it = call.arguments.begin();
			 it != call.arguments.end();
			 ++it) {
			ExprPtr argument(accept(**it, state));
			arguments.push_back(argument);		
		}

		return ExprPtr(
			new CallExpr(call, UndefinedType::singleton(), callee,
			                   arguments));
	}

	virtual ExprPtr visit(ast::BlockExpr& block,
	                            ScopeState state) {
		Scope* scope = new Scope(state.scope); // TODO
		state.scope = scope;

		BlockExpr::expr_list_t exprs;
		for (auto it = block.exprs.begin();
		     it != block.exprs.end();
		     ++it) {
			exprs.push_back(accept(**it, state));
		}

		return ExprPtr(new BlockExpr(block,
		                                         UndefinedType::singleton(),
		                                         scope,
		                                         exprs));
	}

	virtual ExprPtr visit(ast::LiteralNumberExpr& literal,
	                            ScopeState) {
		// TODO: This type is not right. Literal numbers have an unspecified
		//       int type and are implicitly casteable to i32.
		
		TypePtr type(new IntegralType(literal, ast::IntegralType::I32));

		return ExprPtr(new LiteralNumberExpr(literal, type));
	}

	virtual ExprPtr visit(ast::LiteralStringExpr& literal,
	                            ScopeState) {
		TypePtr type(new ArrayType(literal,
				TypePtr(new IntegralType(literal, ast::IntegralType::CHAR))));

		return ExprPtr(new LiteralStringExpr(literal, type));
	}

	virtual ExprPtr visit(ast::LiteralBoolExpr& literal,
	                            ScopeState) {
		TypePtr type(new IntegralType(literal, ast::IntegralType::BOOL));

		return ExprPtr(new LiteralBoolExpr(literal, type));
	}

	virtual ExprPtr visit(ast::VoidExpr& voidExpr,
	                            ScopeState) {
		TypePtr type(new IntegralType(voidExpr,
		                              ast::IntegralType::VOID));
		return ExprPtr(new VoidExpr(voidExpr, type));
	}

	virtual ExprPtr visit(ast::DeclExpr& decl,
                                ScopeState state) {
		DeclPtr decl(accept(*decl.decl, state));
		state.scope->addDecl(decl);

		TypePtr type(new IntegralType(decl, ast::IntegralType::VOID));
		return ExprPtr(
			new DeclExpr(decl, type, decl));
	}	

	virtual ExprPtr visit(ast::IfElseExpr& ifElse,
	                            ScopeState state) {
		return ExprPtr(
			new IfElseExpr(ifElse,
			                     UndefinedType::singleton(),
			                     accept(*ifElse.condition, state),
			                     accept(*ifElse.ifExpr, state),
			                     accept(*ifElse.elseExpr, state)));
	}
};

} // namespace

AstVisitors* makeAstVisitors(Context& context) {
	TypeVisitor* typeVisitor = new TypeVisitor(context);
	DeclVisitor* declVisitor =
		new DeclVisitor(context);
	ExprVisitor* exprVisitor = new ExprVisitor(context);

	AstVisitors* visitors = new AstVisitors(typeVisitor, declVisitor,
	                                        exprVisitor);
	
	typeVisitor->visitors = visitors;
	declVisitor->visitors = visitors;
	exprVisitor->visitors = visitors;

	return visitors;
}

} // namespace semantic
} // namespace llang
