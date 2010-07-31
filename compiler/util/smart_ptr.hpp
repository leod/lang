#ifndef LLANG_UTIL_SMART_PTR_HPP_INCLUDED
#define LLANG_UTIL_SMART_PTR_HPP_INCLUDED

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <utility>

namespace llang {

using boost::scoped_ptr;
using boost::shared_ptr;
using boost::static_pointer_cast;

using std::unique_ptr;

} // namespace llang

#endif
