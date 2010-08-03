#ifndef LLANG_CODEGEN_LLVM_CODEGEN_HPP_INCLUDED
#define LLANG_CODEGEN_LLVM_CODEGEN_HPP_INCLUDED

#include "util/smart_ptr.hpp"
#include "common/context.hpp"
#include "ast/decl.hpp"
#include "ast/visitor.hpp"

namespace llang {
namespace codegen {

class Codegen {
public:
	Codegen(Context&, ast::ModulePtr);
	~Codegen();

	void run();

	struct Impl;
private:
	scoped_ptr<Impl> impl;

};

} // namespace codegen
} // namespace llang

#endif
