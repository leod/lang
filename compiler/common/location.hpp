#ifndef LLANG_COMMON_LOCATION_HPP_INCLUDED
#define LLANG_COMMON_LOCATION_HPP_INCLUDED

#include <iostream>
#include <string>

namespace llang {

struct Location {
	const std::string filename;
	const size_t line;
	const size_t column;

	Location(const std::string& filename, size_t line, size_t column)
		: filename(filename), line(line), column(column) {
	}
};

inline std::ostream& operator<<(std::ostream& os, const Location& location) {
	os << location.filename << ":" << location.line
	   << "(" << location.column << ")";
	return os;
}

} // namespace llang

#endif
