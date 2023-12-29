/*
	Event_WIN32.cpp - 
*/


#include "Event_WIN32.h"


namespace Poco {

EventImpl::EventImpl(bool autoReset) {
	_event = CreateEventW(NULL, autoReset ? FALSE : TRUE, FALSE, NULL);
	if (!_event) {
		// TODO: Handle error.
	}
}


EventImpl::~EventImpl() {
	CloseHandle(_event);
}


bool EventImpl::waitImpl() {
	switch (WaitForSingleObject(_event, INFINITE)) {
	case WAIT_OBJECT_0:
		return true;
	default:
		return false;
	}
}


bool EventImpl::waitImpl(long milliseconds) {
	switch (WaitForSingleObject(_event, milliseconds + 1)) {
	case WAIT_TIMEOUT:
		return false;
	case WAIT_OBJECT_0:
		return true;
	default:
		return false;
	}
}

} // namespace Poco
