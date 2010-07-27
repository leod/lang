#ifndef LLANG_COMMON_LOCATION_HPP_INCLUDED
#define LLANG_COMMON_LOCATION_HPP_INCLUDED

#include <string>

namespace llang {

struct Location {
	const std::string filename;
	const int line;
	const int column;

	Location(const std::string& filename, int line, int column)
		: filename(filename), line(line), column(column) {

	}
};

} // namespace llang

#endif
