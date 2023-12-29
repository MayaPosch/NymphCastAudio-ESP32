/*
	RefCountedObject.cpp 
*/


#include "RefCountedObject.h"


namespace Poco {


RefCountedObject::RefCountedObject(): _counter(1)
{
}


RefCountedObject::~RefCountedObject()
{
}


} // namespace Poco
