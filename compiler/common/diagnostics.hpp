#ifndef LLANG_COMMON_DIAGNOSTICS_HPP_INCLUDED
#define LLANG_COMMON_DIAGNOSTICS_HPP_INCLUDED

#include <string>
#include <cstdarg>
#include "common/location.hpp"
#include "common/config.hpp"

namespace llang {

class Diagnostics {
public:
	Diagnostics(Config&) {}

	void verror(const Location& location, const char* format, va_list argp);
	void error(const Location& location, const char* format, ...);
};

} // namespace llang

#endif
