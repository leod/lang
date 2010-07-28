#include <cstdio>
#include <iostream>

#include "common/diagnostics.hpp"

namespace llang {

void Diagnostics::verror(const Location& location, const char* format, va_list argp) {
	std::cout << location << " - " << std::endl;

	vfprintf(stderr, format, argp);
	fprintf(stderr, "\n");
}

void Diagnostics::error(const Location& location, const char* format, ...) {
	va_list argp;
	va_start(argp, format);
	verror(location, format, argp);
	va_end(argp);
}

} // namespace llang

