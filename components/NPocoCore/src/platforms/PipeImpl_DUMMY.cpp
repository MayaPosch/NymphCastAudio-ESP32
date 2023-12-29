/*
	PipeImpl_DUMMY.cpp 
*/


#include "PipeImpl_DUMMY.h"


namespace Poco {


PipeImpl::PipeImpl()
{
}


PipeImpl::~PipeImpl()
{
}


int PipeImpl::writeBytes(const void* buffer, int length)
{
	return 0;
}


int PipeImpl::readBytes(void* buffer, int length)
{
	return 0;
}


PipeImpl::Handle PipeImpl::readHandle() const
{
	return 0;
}


PipeImpl::Handle PipeImpl::writeHandle() const
{
	return 0;
}


void PipeImpl::closeRead()
{
}


void PipeImpl::closeWrite()
{
}


} // namespace Poco
