/*
	NamedEvent_WIN32.cpp
*/


#include "NamedEvent_WIN32U.h"
//#include "../Error.h"
//#include "Poco/Exception.h"
#include "../Format.h"
#include "../UnicodeConverter.h"


namespace Poco {


NamedEventImpl::NamedEventImpl(const std::string& name):
	_name(name)
{
	UnicodeConverter::toUTF16(_name, _uname);
	_event = CreateEventW(NULL, FALSE, FALSE, _uname.c_str());
	if (!_event)
	{
		//DWORD dwRetVal = GetLastError(); 
		//throw SystemException(format("cannot create named event %s [Error %d: %s]", _name, (int)dwRetVal, Error::getMessage(dwRetVal)));
		// TODO: report error.
	}
}


NamedEventImpl::~NamedEventImpl() {
	CloseHandle(_event);
}


bool NamedEventImpl::setImpl() {
	if (!SetEvent(_event)) { return false; }
		//throw SystemException("cannot signal named event", _name);
		
	return true;
}


bool NamedEventImpl::waitImpl() {
	switch (WaitForSingleObject(_event, INFINITE)) {
	case WAIT_OBJECT_0:
		return true;
	default:
		//throw SystemException("wait for named event failed", _name);
		return false;
	}
}


} // namespace Poco
