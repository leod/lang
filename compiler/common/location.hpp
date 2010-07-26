#ifndef LLANG_COMMON_LOCATION_HPP_INCLUDED
#define LLANG_COMMON_LOCATION_HPP_INCLUDED

#include <string>

namespace llang {

struct Location {
	const std::string file;
	const int line;

	Location(const std::string& file, int line)
		: file(file), line(line) {

	}
};

} // namespace llang

#endif
