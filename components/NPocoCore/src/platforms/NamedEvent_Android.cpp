/*
	NamedEvent_Android.cpp 
*/


#include "NamedEvent_Android.h"
//#include "Poco/Exception.h"


namespace Poco {


NamedEventImpl::NamedEventImpl(const std::string&)
{
}


NamedEventImpl::~NamedEventImpl()
{
}


bool NamedEventImpl::setImpl() {
	//throw NotImplementedException("NamedEvent::set() not available on Android");
	return false;
}


bool NamedEventImpl::waitImpl() {
	//throw NotImplementedException("NamedEvent::wait() not available on Android");
	return false;
}


} // namespace Poco
