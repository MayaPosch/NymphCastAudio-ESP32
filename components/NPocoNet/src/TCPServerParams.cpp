/*
	TCPServerParams.cpp
*/


#include "TCPServerParams.h"


namespace Poco {
namespace Net {


TCPServerParams::TCPServerParams():
	_threadIdleTime(10000000),
	_maxThreads(0),
	_maxQueued(64),
	_threadPriority(Poco::Thread::PRIO_NORMAL)
{
}


TCPServerParams::~TCPServerParams()
{
}


void TCPServerParams::setThreadIdleTime(const Poco::Timespan& milliseconds)
{
	_threadIdleTime = milliseconds;
}


void TCPServerParams::setMaxThreads(int count)
{
	poco_assert (count > 0);

	_maxThreads = count;
}


void TCPServerParams::setMaxQueued(int count)
{
	poco_assert (count >= 0);

	_maxQueued = count;
}


void TCPServerParams::setThreadPriority(Poco::Thread::Priority prio)
{
	_threadPriority = prio;
}


} } // namespace Poco::Net
