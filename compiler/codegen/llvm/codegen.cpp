#include <map>

#define __STDC_LIMIT_MACROS // ?
#define __STDC_CONSTANT_MACROS // ?
#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Analysis/Verifier.h"

#include "ast/type.hpp"
#include "ast/expr.hpp"
#include "ast/visitor.hpp"
#include "codegen/llvm/codegen.hpp"

namespace llang {
namespace codegen {

using namespace ast;
using namespace llvm;

struct ScopeState {
	struct Function {
		llvm::Function* llvmFunction;
		std::map<DeclPtr, Value*> variables;
	};

	// Null outside of functions
	Function* function;

	ScopeState()
		: function(0) {
	}

	ScopeState withFunction(Function* function) const {
		ScopeState result = *this;
		result.function = function;		

		return result;
	}
};

struct Codegen::Impl {
	LLVMContext llvmContext;
	scoped_ptr<llvm::Module> module;
	llvm::IRBuilder<> builder;

	scoped_ptr<Visitor<ScopeState, const llvm::Type*> > typeVisitor;
	scoped_ptr<Visitor<ScopeState, void> > declVisitor;
	scoped_ptr<Visitor<ScopeState, Value*> > exprVisitor;

	ModulePtr moduleDecl;
	
	Impl(Context&, ModulePtr module);
	void run();
};

namespace {

template <typename Result> class VisitorBase
	: public ast::Visitor<ScopeState, Result> {
public:
	Codegen::Impl& visitors;
	Context& context;

	llvm::Module* module;
	LLVMContext& llvmContext;
	IRBuilder<>& builder;

	VisitorBase(Codegen::Impl& visitors, Context& context,
	            llvm::Module* module, IRBuilder<>& builder) :
		visitors(visitors), context(context),
		module(module), llvmContext(module->getContext()),
		builder(builder) {}

	const llvm::Type* accept(TypePtr n, const ScopeState& p) {
		return visitors.typeVisitor->accept(n, p);
	}

	void accept(DeclPtr n, const ScopeState& p) {
		visitors.declVisitor->accept(n, p);
	}

	Value* accept(ExprPtr n, const ScopeState& p) {
		return visitors.exprVisitor->accept(n, p);
	}
};

class TypeVisitor : public VisitorBase<const llvm::Type*> {
public:
	TypeVisitor(Codegen::Impl& visitors, Context& context,
	            llvm::Module* module, IRBuilder<>& builder)
		: VisitorBase<const llvm::Type*>(visitors, context, module, builder) {
	}

protected:
	virtual const llvm::Type* visit(IntegralTypePtr type, ScopeState) {
		switch (type->type) {
		case ast::IntegralType::I32:
			return llvm::Type::getInt32Ty(llvmContext);

		case ast::IntegralType::VOID:
			return llvm::Type::getVoidTy(llvmContext);

		case ast::IntegralType::BOOL:
			return llvm::Type::getInt1Ty(llvmContext);

		default:
			assert(false);
		}
	}

	virtual const llvm::Type* visit(FunctionTypePtr type, ScopeState state) {
		std::vector<const llvm::Type*> params;

		for (auto it = type->parameterTypes.begin();
		     it != type->parameterTypes.end();
		     ++it) {
			params.push_back(accept(*it, state));
		}

		return llvm::FunctionType::get(accept(type->returnType, state),
		                               params, false);
	}
};

class DeclVisitor : public VisitorBase<void> {
public:
	DeclVisitor(Codegen::Impl& visitors, Context& context,
	            llvm::Module* module, IRBuilder<>& builder)
		: VisitorBase<void>(visitors, context, module, builder) {
	}

protected:
	virtual void visit(ModulePtr module, ScopeState state) {
		for (auto it = module->scope->decls.begin();
		     it != module->scope->decls.end();
		     ++it) {
			accept(it->second, state);
		}
	}

	struct SaveInsertPoint {
		SaveInsertPoint(IRBuilder<>& builder)
			: builder(builder),
			  block(builder.GetInsertBlock()),
			  point(builder.GetInsertPoint()) {
		}

		~SaveInsertPoint() {
			builder.SetInsertPoint(block, point);
		}
		
		IRBuilder<>& builder;
		BasicBlock* block;
		BasicBlock::iterator point;
	};

	virtual void visit(FunctionDeclPtr function, ScopeState state) {
		// Need to save insert point as we might already be generating
		// a function right now!
		SaveInsertPoint saveInsertPoint(builder);
		
		// First check if we generated this function already
		// (due to forward references)
		if (module->getFunction(function->mangle())) return;

		const llvm::FunctionType* type =
			llvm::cast<llvm::FunctionType>(accept(function->type, state));
		llvm::Function* f = Function::Create(type,
		                                     Function::ExternalLinkage,
		                                     function->mangle(),
		                                     module);
		
		
		ScopeState::Function functionState;
		functionState.llvmFunction = f;

		{
			assert(f->arg_size() == function->parameters.size()); // TODO

			auto it1 = f->arg_begin();
			auto it2 = function->parameters.begin();
			for (; it1 != f->arg_end(); ++it1, ++it2) {
				//if (!it2->decl) continue; // unnamed parameter

				it1->setName((*it2)->name);
				functionState.variables[*it2] = it1;
			}
		}

		BasicBlock* block = BasicBlock::Create(llvmContext, "entry", f);
		builder.SetInsertPoint(block);

		IntegralTypePtr returnType = isA<IntegralType>(function->returnType);

		Value* bodyValue = accept(function->body,
		                          state.withFunction(&functionState));

		if (!returnType || returnType->type != ast::IntegralType::VOID) {
			assert(bodyValue);

			builder.CreateRet(bodyValue);
		}
		else
			builder.CreateRetVoid();

		llvm::verifyFunction(*f);
	}

	virtual void visit(VariableDeclPtr variable, ScopeState state) {
		assert(state.function); // TODO

		llvm::Function* llvmFunction = state.function->llvmFunction;

		IRBuilder<> entryBuilder(&llvmFunction->getEntryBlock(),
		                         llvmFunction->getEntryBlock().begin());
		AllocaInst* alloca = entryBuilder.CreateAlloca(
			accept(variable->type, state), 0, variable->name);
		assert(alloca);
	
		Value* init = accept(variable->initializer, state);
		builder.CreateStore(init, alloca);

		state.function->variables[variable] = alloca;
	}
};

class ExprVisitor : public VisitorBase<Value*> {
public:
	ExprVisitor(Codegen::Impl& visitors, Context& context,
	            llvm::Module* module, IRBuilder<>& builder)
		: VisitorBase<Value*>(visitors, context, module, builder) {
	}

private:
	Function* getFunction(FunctionDeclPtr function, ScopeState state) {
		if (Function* llvmFunction = module->getFunction(function->mangle())) {
			return llvmFunction;
		}

		accept(function, state);

		Function* llvmFunction = module->getFunction(function->mangle());
		assert(llvmFunction);

		return llvmFunction;
	}

protected:
	virtual Value* visit(DeclRefExprPtr expr, ScopeState state) {
		Value* value = 0;

		if (ParameterDeclPtr decl =
			isA<ParameterDecl>(DeclPtr(expr->decl))) {
			value = state.function->variables[decl];
		}
		else if (VariableDeclPtr decl =
			isA<VariableDecl>(DeclPtr(expr->decl))) {
			value = builder.CreateLoad(state.function->variables[decl],
			                           decl->name);
		}
		else assert(false); // TODO

		assert(value);
		return value;
	}

	virtual Value* visit(CallExprPtr expr, ScopeState state) {
		if (DeclRefExprPtr callee =
				isA<DeclRefExpr>(expr->callee)) {
			if (FunctionDeclPtr function =
					isA<FunctionDecl>(DeclPtr(callee->decl))) {
				Function* llvmFunction = getFunction(function, state);
				assert(llvmFunction);

				std::vector<Value*> arguments;
				for (auto it = expr->arguments.begin();
				     it != expr->arguments.end();
				     ++it) {
					arguments.push_back(accept(*it, state));
				}

				return builder.CreateCall(llvmFunction, arguments.begin(),
				                          arguments.end(), "calltmp");
			}
			else assert(false); // TODO
		}
		else assert(false); // TODO
	}

	virtual Value* visit(VoidExprPtr, ScopeState) {
		return 0;
	}

	virtual Value* visit(BlockExprPtr block, ScopeState state) {
		Value* value = 0;
		for (auto it = block->exprs.begin();
		     it != block->exprs.end();
		     ++it) {
			value = accept(*it, state);	
		}

		return value;
	}		
		
	virtual Value* visit(LiteralNumberExprPtr expr, ScopeState) {
		return ConstantInt::get(llvmContext,
			APInt(sizeof(int_t) * 8, expr->number, true));
	}

	virtual Value* visit(LiteralStringExprPtr expr, ScopeState) {
		size_t length = expr->string.size();

		const llvm::Type* elementType = IntegerType::get(llvmContext, 8);
		const llvm::Type* type = llvm::ArrayType::get(elementType, length);
		llvm::StringRef string = StringRef(expr->string);
		llvm::Constant* init = ConstantArray::get(llvmContext, string, false);

		GlobalVariable* global =
			new GlobalVariable(*module, type, true, GlobalValue::InternalLinkage,
			                   init, "string");
		return global;
	}

	virtual Value* visit(LiteralBoolExprPtr expr, ScopeState) {
		return expr->value ? ConstantInt::getTrue(llvmContext)
		                   : ConstantInt::getFalse(llvmContext);
	}
	
	virtual Value* visit(BinaryExprPtr expr, ScopeState state) {
		Value* left  = accept(expr->left, state);
		Value* right = accept(expr->right, state);

		assert(left);
		assert(right);

		switch (expr->operation) {
		case ast::BinaryExpr::ADD:
			return builder.CreateAdd(left, right, "addtmp");

		case ast::BinaryExpr::SUB:
			return builder.CreateSub(left, right, "subtmp");

		case ast::BinaryExpr::MUL:
			return builder.CreateMul(left, right, "multmp");

		//case ast::BinaryExpr::DIV:
			//return builder.CreateSDiv(left, right, "divtmp");

		case ast::BinaryExpr::EQUALS:
			return builder.CreateICmpEQ(left, right, "eqtmp");

		default:
			assert(false);
		}
	}

	virtual Value* visit(DeclExprPtr expr, ScopeState state) {
		accept(DeclPtr(expr->decl), state);
		return 0; // TODO?
	}

	virtual Value* visit(IfElseExprPtr expr, ScopeState state) {
		Value* condition = accept(expr->condition, state);

		assert(state.function);
		Function* llvmFunction = state.function->llvmFunction;
		assert(llvmFunction);

		BasicBlock* ifBlock = BasicBlock::Create(llvmContext, "ifblock",
		                                         llvmFunction);
		BasicBlock* elseBlock = BasicBlock::Create(llvmContext, "elseblock");
		BasicBlock* mergeBlock = BasicBlock::Create(llvmContext, "mergeblock");

		builder.CreateCondBr(condition, ifBlock, elseBlock);

		builder.SetInsertPoint(ifBlock);
		Value* ifValue = accept(expr->ifExpr, state);
		builder.CreateBr(mergeBlock);
		ifBlock = builder.GetInsertBlock();

		llvmFunction->getBasicBlockList().push_back(elseBlock);
		builder.SetInsertPoint(elseBlock);
		Value* elseValue = accept(expr->elseExpr, state);
		builder.CreateBr(mergeBlock);
		elseBlock = builder.GetInsertBlock();

		llvmFunction->getBasicBlockList().push_back(mergeBlock);
		builder.SetInsertPoint(mergeBlock);
		PHINode* phi = builder.CreatePHI(accept(expr->type, state),
		                                 "iftmp");
		phi->addIncoming(ifValue, ifBlock);
		phi->addIncoming(elseValue, elseBlock);

		return phi;
	}
};

} // namespace

Codegen::Impl::Impl(Context& context, ModulePtr moduleDecl)
	: llvmContext(),
	  module(new llvm::Module("test", llvmContext)),
	  builder(llvmContext),
	  typeVisitor(new TypeVisitor(*this, context, module.get(), builder)),
	  declVisitor(new DeclVisitor(*this, context, module.get(), builder)),
	  exprVisitor(new ExprVisitor(*this, context, module.get(), builder)),
	  moduleDecl(moduleDecl) {
}

void Codegen::Impl::run() {
	ScopeState state;
	declVisitor->accept(moduleDecl, state);
}

Codegen::Codegen(Context& context, ModulePtr moduleDecl)
	: impl(new Impl(context, moduleDecl)) {
}

Codegen::~Codegen() {

}

void Codegen::run() {
	impl->run();
	impl->module->dump();
}

} // namespace codegen
} // namespace llang

