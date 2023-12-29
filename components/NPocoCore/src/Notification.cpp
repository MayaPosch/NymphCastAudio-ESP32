/*
	Notification.cpp 
*/


#include "Notification.h"
#include <typeinfo>


namespace Poco {


Notification::Notification()
{
}


Notification::~Notification()
{
}


std::string Notification::name() const
{
	return typeid(*this).name();
}


} // namespace Poco
