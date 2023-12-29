/*
	Mutex_VX.h - Definition of the MutexImpl and FastMutexImpl classes for VxWorks.
*/


#ifndef Core_Mutex_VX_INCLUDED
#define Core_Mutex_VX_INCLUDED


#include "../Core.h"
#include <semLib.h>
#include <errno.h>


namespace Poco {


class Foundation_API MutexImpl {
protected:
	MutexImpl();
	MutexImpl(bool fast);
	~MutexImpl();
	bool lockImpl();
	bool tryLockImpl();
	bool tryLockImpl(long milliseconds);
	void unlockImpl();
	
private:
	SEM_ID _sem;
};


class Foundation_API FastMutexImpl: public MutexImpl {
protected:
	FastMutexImpl();
	~FastMutexImpl();
};


// inlines
inline bool MutexImpl::lockImpl() {
	if (semTake(_sem, WAIT_FOREVER) != OK) {
		return false;
	}
	
	return true;
}


inline bool MutexImpl::tryLockImpl() {
	return semTake(_sem, NO_WAIT) == OK;
}


inline void MutexImpl::unlockImpl() {
	if (semGive(_sem) != OK) {
		// TODO: Non-fatal, could fail if thread does not own mutex.
	}
}


} // namespace Poco


#endif // Core_Mutex_VX_INCLUDED
