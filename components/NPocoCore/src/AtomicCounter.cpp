/*
	AtomicCounter.cpp -
*/


#include "AtomicCounter.h"


namespace Poco {

AtomicCounter::AtomicCounter(): _counter(0) { }

	
AtomicCounter::AtomicCounter(AtomicCounter::ValueType initialValue): 
	_counter(initialValue)
{
}


AtomicCounter::AtomicCounter(const AtomicCounter& counter):
	_counter(counter.value())
{
}


AtomicCounter::~AtomicCounter() { }


AtomicCounter& AtomicCounter::operator = (const AtomicCounter& counter) {
	_counter.store(counter._counter.load());
	return *this;
}

	
AtomicCounter& AtomicCounter::operator = (AtomicCounter::ValueType value) {
	_counter.store(value);
	return *this;
}

} // namespace Poco
