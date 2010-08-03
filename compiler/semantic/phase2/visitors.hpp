#ifndef LLANG_SEMANTIC_PHASE2_VISITORS_HPP_INCLUDED
#define LLANG_SEMANTIC_PHASE2_VISITORS_HPP_INCLUDED

#include <list>

#include "common/context.hpp"
#include "semantic/visitor.hpp"

namespace llang {
namespace semantic {

Visitors* makePhase2Visitors(Context&);

} // namespace semantic
} // namespace llang

#endif

