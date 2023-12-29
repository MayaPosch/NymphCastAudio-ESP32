/*
	OrderedSet.h - Definition of the OrderedSet class template.
*/


#ifndef Core_OrderedSet_INCLUDED
#define Core_OrderedSet_INCLUDED


#include "Core.h"


#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include "ordered_hash.h"
#include "ordered_set.h"


namespace Poco {


template<class Key,
		class Hash = std::hash<Key>,
		class KeyEqual = std::equal_to<Key>,
		class Allocator = std::allocator<Key>,
		class ValueTypeContainer = std::deque<Key, Allocator>>
using OrderedSet = tsl::ordered_set<Key, Hash, KeyEqual, Allocator, ValueTypeContainer>;
	/// For documentation, see https://tessil.github.io/ordered-map/

} // namespace Poco


#endif // Core_OrderedSet_INCLUDED
