/*
	AbstractObserver.cpp 
*/


#include "AbstractObserver.h"


namespace Poco {


AbstractObserver::AbstractObserver()
{
}


AbstractObserver::AbstractObserver(const AbstractObserver& /*observer*/)
{
}


AbstractObserver::~AbstractObserver()
{
}

	
AbstractObserver& AbstractObserver::operator = (const AbstractObserver& /*observer*/)
{
	return *this;
}


} // namespace Poco
