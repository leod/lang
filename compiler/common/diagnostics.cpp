#include <cstdio>
#include <iostream>
#include <stdexcept>

#include "common/diagnostics.hpp"

namespace llang {

void Diagnostics::verror(const Location& location, const char* format, va_list argp) {
	std::cout << location << ": error: " << std::flush;

	vfprintf(stderr, format, argp);
	fprintf(stderr, "\n");

	// TODO: throw something else
	throw std::runtime_error("error");
}

void Diagnostics::error(const Location& location, const char* format, ...) {
	va_list argp;
	va_start(argp, format);
	verror(location, format, argp);
	va_end(argp);
}

} // namespace llang

