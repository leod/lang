#ifndef LLANG_SEMANTIC_PHASE1_VISITORS_HPP_INCLUDED
#define LLANG_SEMANTIC_PHASE1_VISITORS_HPP_INCLUDED

#include "common/context.hpp"
#include "semantic/visitor.hpp"

namespace llang {
namespace semantic {

Visitors* makePhase1Visitors(Context&);

} // namespace semantic
} // namespace llang

#endif
