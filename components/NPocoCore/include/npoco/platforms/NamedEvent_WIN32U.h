/*
	NamedEvent_WIN32U.h - Definition of the NamedEventImpl class for Windows.
*/


#ifndef Core_NamedEvent_WIN32U_INCLUDED
#define Core_NamedEvent_WIN32U_INCLUDED


#include "../Core.h"
#include "UnWindows.h"


namespace Poco {


class Foundation_API NamedEventImpl
{
protected:
	NamedEventImpl(const std::string& name);	
	~NamedEventImpl();
	bool setImpl();
	bool waitImpl();
	
private:
	std::string  _name;
	std::wstring _uname;
	HANDLE      _event;	
};


} // namespace Poco


#endif // Core_NamedEvent_WIN32U_INCLUDED
