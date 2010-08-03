#ifndef LLANG_CODEGEN_LLVM_CODEGEN_HPP_INCLUDED
#define LLANG_CODEGEN_LLVM_CODEGEN_HPP_INCLUDED

#include "util/smart_ptr.hpp"
#include "common/context.hpp"
#include "semantic/decl.hpp"
#include "semantic/visitor.hpp"

namespace llang {
namespace codegen {

class Codegen {
public:
	Codegen(Context&, semantic::ModulePtr);
	~Codegen();

	void run();

	struct Impl;
private:
	scoped_ptr<Impl> impl;

};

} // namespace codegen
} // namespace llang

#endif
