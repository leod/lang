#ifndef LLANG_COMMON_CONTEXT_HPP_INCLUDED
#define LLANG_COMMON_CONTEXT_HPP_INCLUDED

#include "common/config.hpp"
#include "common/diagnostics.hpp"

namespace llang {

struct Context {
	Context(const Config& config, Diagnostics& diag)
		: config(config), diag(diag) {
	}

	const Config& config;
	Diagnostics& diag;
};

} // namespace llang

#endif

