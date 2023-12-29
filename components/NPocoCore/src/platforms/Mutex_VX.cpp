/*
	Mutex_VX.cpp
*/


#include "Mutex_VX.h"
#include <sysLib.h>


namespace Poco {


MutexImpl::MutexImpl() {
	_sem = semMCreate(SEM_INVERSION_SAFE | SEM_Q_PRIORITY);
	if (_sem == 0) {
		// TODO: handle error.
	}
}


MutexImpl::MutexImpl(bool fast) {
	if (fast) {
		_sem = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
	}
	else {
		_sem = semMCreate(SEM_INVERSION_SAFE | SEM_Q_PRIORITY);
	}
	
	if (_sem == 0) {
		// TODO: handle error.
	}
}


MutexImpl::~MutexImpl() {
	semDelete(_sem);
}


bool MutexImpl::tryLockImpl(long milliseconds) {
	int ticks = milliseconds*sysClkRateGet()/1000;
	return semTake(_sem, ticks) == OK;
}


FastMutexImpl::FastMutexImpl(): MutexImpl(true) {
}


FastMutexImpl::~FastMutexImpl() {
}


} // namespace Poco
