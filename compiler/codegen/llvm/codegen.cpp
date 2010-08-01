#include <map>

#define __STDC_LIMIT_MACROS // ?
#define __STDC_CONSTANT_MACROS // ?
#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Analysis/Verifier.h"

#include "semantic/visitor.hpp"
#include "codegen/llvm/codegen.hpp"

namespace llang {
namespace codegen {

using namespace semantic;
using namespace llvm;

struct ScopeState {
	struct {
		std::map<SymbolPtr, Value*> variables;
	} function;
};

struct Codegen::Impl {
	LLVMContext llvmContext;
	scoped_ptr<llvm::Module> module;
	llvm::IRBuilder<> builder;

	scoped_ptr<Visitor<ScopeState, const llvm::Type*> > typeVisitor;
	scoped_ptr<Visitor<ScopeState, void> > symbolVisitor;
	scoped_ptr<Visitor<ScopeState, Value*> > expressionVisitor;

	ModulePtr moduleSymbol;
	
	Impl(Context&, ModulePtr module);
	void run();
};

template <typename Result> class VisitorBase
	: public Visitor<ScopeState, Result> {
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

	void accept(SymbolPtr n, const ScopeState& p) {
		visitors.symbolVisitor->accept(n, p);
	}

	Value* accept(ExpressionPtr n, const ScopeState& p) {
		return visitors.expressionVisitor->accept(n, p);
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

class SymbolVisitor : public VisitorBase<void> {
public:
	SymbolVisitor(Codegen::Impl& visitors, Context& context,
	              llvm::Module* module, IRBuilder<>& builder)
		: VisitorBase<void>(visitors, context, module, builder) {
	}

protected:
	virtual void visit(ModulePtr module, ScopeState state) {
		for (auto it = module->scope->symbols.begin();
		     it != module->scope->symbols.end();
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

	virtual void visit(FunctionSymbolPtr function, ScopeState state) {
		// Need to save insert point as we might already be generating
		// a function right now!
		SaveInsertPoint saveInsertPoint(builder);
		
		// First check if we generated this function already
		// (due to forward references)
		if (module->getFunction(function->name)) return;

		const llvm::FunctionType* type =
			llvm::cast<llvm::FunctionType>(accept(function->type, state));
		llvm::Function* f = Function::Create(type,
		                                     Function::ExternalLinkage,
		                                     function->name,
		                                     module);

		BasicBlock* block = BasicBlock::Create(llvmContext, "entry", f);
		builder.SetInsertPoint(block);

		IntegralTypePtr returnType = isA<IntegralType>(function->returnType);
		
		Value* bodyValue = accept(function->body, state);

		if (!returnType || returnType->type != ast::IntegralType::VOID) {
			builder.CreateRet(bodyValue);
			llvm::verifyFunction(*f);
		}
	}
};

class ExpressionVisitor : public VisitorBase<Value*> {
public:
	ExpressionVisitor(Codegen::Impl& visitors, Context& context,
	                  llvm::Module* module, IRBuilder<>& builder)
		: VisitorBase<Value*>(visitors, context, module, builder) {
	}

private:
	Function* getFunction(FunctionSymbolPtr function, ScopeState state) {
		if (Function* llvmFunction = module->getFunction(function->name)) {
			return llvmFunction;
		}

		accept(function, state);

		Function* llvmFunction = module->getFunction(function->name);
		assert(llvmFunction);

		return llvmFunction;
	}

protected:
	virtual Value* visit(CallExpressionPtr expression, ScopeState state) {
		if (SymbolExpressionPtr callee =
				isA<SymbolExpression>(expression->callee)) {
			if (FunctionSymbolPtr function =
					isA<FunctionSymbol>(SymbolPtr(callee->symbol))) {
				Function* llvmFunction = getFunction(function, state);
				assert(llvmFunction);

				std::vector<Value*> arguments;
				for (auto it = expression->arguments.begin();
				     it != expression->arguments.end();
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

	virtual Value* visit(VoidExpressionPtr, ScopeState) {
		return 0;
	}

	virtual Value* visit(BlockExpressionPtr block, ScopeState state) {
		Value* value = 0;
		for (auto it = block->expressions.begin();
		     it != block->expressions.end();
		     ++it) {
			value = accept(*it, state);	
		}

		return value;
	}		
		
	virtual Value* visit(LiteralNumberExpressionPtr expression, ScopeState) {
		return ConstantInt::get(llvmContext,
			APInt(sizeof(int_t) * 8, expression->number, true));
	}
	
	virtual Value* visit(BinaryExpressionPtr expression, ScopeState state) {
		Value* left  = accept(expression->left, state);
		Value* right = accept(expression->right, state);

		switch (expression->operation) {
		case ast::BinaryExpression::ADD:
			return builder.CreateFAdd(left, right, "addtmp");

		case ast::BinaryExpression::SUB:
			return builder.CreateFSub(left, right, "subtmp");

		case ast::BinaryExpression::MUL:
			return builder.CreateFMul(left, right, "multmp");

		case ast::BinaryExpression::DIV:
			return builder.CreateFDiv(left, right, "divtmp");

		default:
			assert(false);
		}
	}
};

Codegen::Impl::Impl(Context& context, ModulePtr moduleSymbol)
	: llvmContext(),
	  module(new llvm::Module(llvm::StringRef("test"), llvmContext)),
	  builder(llvmContext),
	  typeVisitor(new TypeVisitor(*this, context, module.get(), builder)),
	  symbolVisitor(new SymbolVisitor(*this, context, module.get(), builder)),
	  expressionVisitor(new ExpressionVisitor(*this, context, module.get(),
	                    builder)),
	  moduleSymbol(moduleSymbol) {
}

void Codegen::Impl::run() {
	ScopeState state;
	symbolVisitor->accept(moduleSymbol, state);
}

Codegen::Codegen(Context& context, ModulePtr moduleSymbol)
	: impl(new Impl(context, moduleSymbol)) {
}

Codegen::~Codegen() {

}

void Codegen::run() {
	impl->run();
	impl->module->dump();
}

} // namespace codegen
} // namespace llang

