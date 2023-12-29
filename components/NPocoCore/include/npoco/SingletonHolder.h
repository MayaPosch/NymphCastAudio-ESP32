/*
	SingletonHolder.h - Definition of the SingletonHolder template.
*/


#ifndef Core_SingletonHolder_INCLUDED
#define Core_SingletonHolder_INCLUDED


#include "Core.h"
#include "Mutex.h"


namespace Poco {

/// This is a helper template class for managing
/// singleton objects allocated on the heap.
/// The class ensures proper deletion (including
/// calling of the destructor) of singleton objects
/// when the application that created them terminates.
template <class S>
class SingletonHolder {
public:
	/// Creates the SingletonHolder.
	SingletonHolder(): _pS(0) { }
	
	/// Destroys the SingletonHolder and the singleton
	/// object that it holds.
	~SingletonHolder() {
		delete _pS;
	}
	
	/// Returns a pointer to the singleton object held by the SingletonHolder. The first call
	/// to get will create the singleton.
	S* get() {
		FastMutex::ScopedLock lock(_m);
		if (!_pS) _pS = new S;
		return _pS;
	}
	
	/// Deletes the singleton object.
	void reset() {
		FastMutex::ScopedLock lock(_m);
		delete _pS;
		_pS = 0;
	}
	
private:
	S* _pS;
	FastMutex _m;
};

} // namespace Poco


#endif // Core_SingletonHolder_INCLUDED
