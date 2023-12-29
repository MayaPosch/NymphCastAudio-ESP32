/*
	RWLock_WINCE.cpp - 
*/


#include "RWLock_WINCE.h"
#include "../Thread.h"


namespace Poco {

RWLockImpl::RWLockImpl(): 
	_readerCount(0),
	_readerWaiting(0),
	_writerCount(0),
	_writerWaiting(0),
	_writeLock(false)
	
{
	InitializeCriticalSection(&_cs);
	_readerGreen = CreateEventW(NULL, FALSE, TRUE, NULL);
	if (!_readerGreen) {
		//throw SystemException("Cannot create RWLock");
		// TODO: Handle error.
	}
	
	_writerGreen = CreateEventW(NULL, FALSE, TRUE, NULL);
	if (!_writerGreen) {
		CloseHandle(_readerGreen);
		//throw SystemException("Cannot create RWLock");
		// TODO: Handle error.
	}
}


RWLockImpl::~RWLockImpl() {
	CloseHandle(_readerGreen);
	CloseHandle(_writerGreen);
	DeleteCriticalSection(&_cs);
}


bool RWLockImpl::readLockImpl() {
	return tryReadLockImpl(INFINITE);
}


bool RWLockImpl::tryReadLockImpl(DWORD timeout) {
	bool wait = false;
	do {
		EnterCriticalSection(&_cs);
		if (!_writerCount && !_writerWaiting) {
			if (wait) {
				_readerWaiting--;
				wait = false;
			}
			_readerCount++;
		}
		else {
			if (!wait) {
				_readerWaiting++;
				wait = true;
			}
			
			ResetEvent(_readerGreen);
		}
		
		LeaveCriticalSection(&_cs); 
		if (wait) {
			if (WaitForSingleObject(_readerGreen, timeout) != WAIT_OBJECT_0) {
				EnterCriticalSection(&_cs);
				_readerWaiting--;
				SetEvent(_readerGreen); 
				SetEvent(_writerGreen);
				LeaveCriticalSection(&_cs);
				return false;
			}
		}
	} 
	while (wait);
   
	return true;
}


void RWLockImpl::writeLockImpl() {
	return tryWriteLockImpl(INFINITE);
}


bool RWLockImpl::tryWriteLockImpl(DWORD timeout) {
	bool wait = false;

	do {
		EnterCriticalSection(&_cs);
		if (!_readerCount && !_writerCount) {
			if (wait) {
				_writerWaiting--;
				wait = false;
			}
			
			_writerCount++;
		}
		else {
			if (!wait) {
				_writerWaiting++;
				wait = true;
			}
			
			ResetEvent(_writerGreen);
		}
		
		LeaveCriticalSection(&_cs);
		if (wait) {
			if (WaitForSingleObject(_writerGreen, timeout) != WAIT_OBJECT_0) {
				EnterCriticalSection(&_cs);
				_writerWaiting--;
				SetEvent(_readerGreen);
				SetEvent(_writerGreen);
				LeaveCriticalSection(&_cs);
				return false;
			}
		}
	}
	while (wait);
	
	_writeLock = true;
	return true;
}


bool RWLockImpl::unlockImpl() {
	EnterCriticalSection(&_cs);
	
	if (_writeLock) {
		_writeLock = false;
		_writerCount--;
	}
	else {
		_readerCount--;
	}
	
	if (_writerWaiting)
		SetEvent(_writerGreen);
	else if (_readerWaiting)
		SetEvent(_readerGreen);   
		  
	LeaveCriticalSection(&_cs);
	return true;
}


} // namespace Poco
