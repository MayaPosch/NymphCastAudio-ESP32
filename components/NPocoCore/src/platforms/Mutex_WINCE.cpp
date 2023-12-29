/*
	Mutex_WINCE.cpp - 
*/


#include "Mutex_WINCE.h"


namespace Poco {


MutexImpl::MutexImpl() {
	_mutex = CreateMutexW(NULL, FALSE, NULL);
	//if (!_mutex) throw SystemException("cannot create mutex");
}


MutexImpl::~MutexImpl() {
	CloseHandle(_mutex);
}


bool MutexImpl::lockImpl() {
	switch (WaitForSingleObject(_mutex, INFINITE)) {
	case WAIT_OBJECT_0:
		return true;
	default:
		//throw SystemException("cannot lock mutex");
		return false;
	}
}


bool MutexImpl::tryLockImpl() {
	switch (WaitForSingleObject(_mutex, 0))
	{
	case WAIT_TIMEOUT:
		return false;
	case WAIT_OBJECT_0:
		return true;
	default:
		//throw SystemException("cannot lock mutex");		
		return false;
	}
}


bool MutexImpl::tryLockImpl(long milliseconds) {
	switch (WaitForSingleObject(_mutex, milliseconds + 1)) {
	case WAIT_TIMEOUT:
		return false;
	case WAIT_OBJECT_0:
		return true;
	default:
		//throw SystemException("cannot lock mutex");
		return false;
	}
}


void MutexImpl::unlockImpl() {
	ReleaseMutex(_mutex);
}


} // namespace Poco
