/* This is a template include used for creating base classes for visitors over a
   fixed set of classes.

   It expects the following macros to be set:
       LLANG_VISITOR_TABLE_PARAM(apply): Takes a macro and applies it to every
                                         class type the visitor is supposed to
                                         work with.
       LLANG_VISITOR_TYPE_PARAM: The base type of all classes.
       LLANG_VISITOR_TAG_PARAM: A member of the base type returning a tag.
*/

#include <cassert>

// Generate forward references
#define GENERATE_FORWARD_REFERENCE(name, nameInCaps) \
	class name;
LLANG_VISITOR_TABLE_PARAM(GENERATE_FORWARD_REFERENCE)
#undef GENERATE_FORWARD_REFERENCE

#define GENERATE_VIRTUAL_METHOD(name, nameCaps) \
	virtual Result visit(name&, Param) { assert(false); }

#define GENERATE_CASE(name, nameCaps) \
	case LLANG_VISITOR_TYPE_PARAM::nameCaps: \
		return visit(static_cast<name&>(node), param);

template <typename Param, typename Result> class Visitor {
protected:
	// Generate virtual methods
	LLANG_VISITOR_TABLE_PARAM(GENERATE_VIRTUAL_METHOD)

public:
	virtual ~Visitor() {}

	Result accept(LLANG_VISITOR_TYPE_PARAM& node, Param param) {
		switch(node.LLANG_VISITOR_TAG_PARAM) {

		// Generate cases
		LLANG_VISITOR_TABLE_PARAM(GENERATE_CASE)

		default:
			assert(false);
		}
	}
};

#undef GENERATE_VIRTUAL_METHOD	
#undef GENERATE_CASE

#define GENERATE_VIRTUAL_METHOD(name, nameCaps) \
	virtual Result visit(name&) { assert(false); }

#define GENERATE_CASE(name, nameCaps) \
	case LLANG_VISITOR_TYPE_PARAM::nameCaps: \
		return visit(static_cast<name&>(node)); 

template <typename Result> class Visitor <void, Result> {
protected:
	LLANG_VISITOR_TABLE_PARAM(GENERATE_VIRTUAL_METHOD)

public:
	virtual ~Visitor() {}

	Result accept(LLANG_VISITOR_TYPE_PARAM& node) {
		switch(node.LLANG_VISITOR_TAG_PARAM) {

		LLANG_VISITOR_TABLE_PARAM(GENERATE_CASE)

		default:
			assert(false);
		}
	}
};

#undef GENERATE_VIRTUAL_METHOD	
#undef GENERATE_CASE
