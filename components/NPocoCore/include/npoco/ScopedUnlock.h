/*
	ScopedUnlock.h - Definition of the ScopedUnlock template class.
*/


#ifndef Core_ScopedUnlock_INCLUDED
#define Core_ScopedUnlock_INCLUDED


#include "Core.h"


namespace Poco {

/// A class that simplifies thread synchronization
/// with a mutex.
/// The constructor accepts a Mutex and unlocks it.
/// The destructor locks the mutex.
template <class M>
class ScopedUnlock {
public:
	inline ScopedUnlock(M& mutex, bool unlockNow = true): _mutex(mutex) {
		if (unlockNow) {
			_mutex.unlock();
		}
	}
	
	inline ~ScopedUnlock() {
		_mutex.lock();
	}

private:
	M& _mutex;

	ScopedUnlock();
	ScopedUnlock(const ScopedUnlock&);
	ScopedUnlock& operator = (const ScopedUnlock&);
};


} // namespace Poco


#endif // Core_ScopedUnlock_INCLUDED
