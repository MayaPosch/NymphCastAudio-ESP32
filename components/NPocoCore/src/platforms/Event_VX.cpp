/*
	Event_POSIX.cpp - 
*/


#include "Event_VX.h"
#include <sysLib.h>


namespace Poco {


EventImpl::EventImpl(bool autoReset): _auto(autoReset), _state(false) {
	_sem = semCCreate(SEM_Q_PRIORITY, 0);
	if (_sem == 0) {
		// TODO: Handle error.
	}
}


EventImpl::~EventImpl() {
	semDelete(_sem);
}


bool EventImpl::setImpl() {
	if (_auto) {
		if (semGive(_sem) != OK) {
			return false;
		}
	}
	else {
		_state = true;
		if (semFlush(_sem) != OK) { return false; }
	}
	
	return true;
}


bool EventImpl::resetImpl() {
	_state = false;
	
	return true;
}


bool EventImpl::waitImpl() {
	if (!_state) {
		if (semTake(_sem, WAIT_FOREVER) != OK) { return false; }
	}
	
	return true;
}


bool EventImpl::waitImpl(long milliseconds) {
	if (!_state) {
		int ticks = milliseconds*sysClkRateGet() / 1000;
		return semTake(_sem, ticks) == OK;
	}
	
	return true;
}


} // namespace Poco
