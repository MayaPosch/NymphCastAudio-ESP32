/*
	SignalHandler.cpp - 
*/


#include "SignalHandler.h"


#if defined(POCO_OS_FAMILY_UNIX) && !defined(POCO_VXWORKS)


#include "Thread.h"
#include "NumberFormatter.h"
#include <cstdlib>
#include <signal.h>


namespace Poco {


SignalHandler::JumpBufferVec SignalHandler::_jumpBufferVec;


SignalHandler::SignalHandler()
{
	JumpBufferVec& jbv = jumpBufferVec();
	JumpBuffer buf;
	jbv.push_back(buf);
}


SignalHandler::~SignalHandler()
{
	jumpBufferVec().pop_back();
}


sigjmp_buf& SignalHandler::jumpBuffer()
{
	return jumpBufferVec().back().buf;
}


void SignalHandler::throwSignalException(int sig) {
	// TODO: implement something?
	/* switch (sig) {
	case SIGILL:
		//throw SignalException("Illegal instruction");
	case SIGBUS:
		//throw SignalException("Bus error"); 
	case SIGSEGV:
		//throw SignalException("Segmentation violation");
	case SIGSYS:
		//throw SignalException("Invalid system call");
	default:
		//throw SignalException(NumberFormatter::formatHex(sig));
	} */
}


void SignalHandler::install()
{
#ifndef POCO_NO_SIGNAL_HANDLER
	struct sigaction sa;
	sa.sa_handler = handleSignal;
	sa.sa_flags   = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGILL,  &sa, 0);
	sigaction(SIGBUS,  &sa, 0);
	sigaction(SIGSEGV, &sa, 0);
	sigaction(SIGSYS,  &sa, 0);
#endif
}


void SignalHandler::handleSignal(int sig)
{
	JumpBufferVec& jb = jumpBufferVec();
	if (!jb.empty())
		siglongjmp(jb.back().buf, sig);
		
	// Abort if no jump buffer registered
	std::abort();
}


SignalHandler::JumpBufferVec& SignalHandler::jumpBufferVec()
{
	ThreadImpl* pThread = ThreadImpl::currentImpl();
	if (pThread)
		return pThread->_jumpBufferVec;
	else
		return _jumpBufferVec;
}


} // namespace Poco


#endif // POCO_OS_FAMILY_UNIX
