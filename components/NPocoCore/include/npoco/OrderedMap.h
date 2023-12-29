/*
	OrderedMap.h - Definition of the OrderedMap class template.
*/


#ifndef Core_OrderedMap_INCLUDED
#define Core_OrderedMap_INCLUDED


#include "Core.h"


#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include "ordered_hash.h"
#include "ordered_map.h"


namespace Poco {


template<class Key,
		class T,
		class Hash = std::hash<Key>,
		class KeyEqual = std::equal_to<Key>,
		class Allocator = std::allocator<std::pair<Key, T>>,
		class ValueTypeContainer = std::deque<std::pair<Key, T>, Allocator>>
using OrderedMap = tsl::ordered_map<Key, T, Hash, KeyEqual, Allocator, ValueTypeContainer>;
	/// For documentation, see https://tessil.github.io/ordered-map/


} // namespace Poco


#endif // Core_OrderedMap_INCLUDED
