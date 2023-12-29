/*
	Process_VX.cpp 
*/


#include "Process_VX.h"
//#include "Poco/Exception.h"


namespace Poco {


//
// ProcessHandleImpl
//
ProcessHandleImpl::ProcessHandleImpl(int pid):
	_pid(pid)
{
}


ProcessHandleImpl::~ProcessHandleImpl()
{
}


int ProcessHandleImpl::id() const
{
	return _pid;
}


int ProcessHandleImpl::wait() const {
	//throw Poco::NotImplementedException("Process::wait()");
	return -1;
}


int ProcessHandleImpl::tryWait() const {
	//throw Poco::NotImplementedException("Process::tryWait()");
	return -1;
}


//
// ProcessImpl
//
ProcessImpl::PIDImpl ProcessImpl::idImpl()
{
	return 0;
}


void ProcessImpl::timesImpl(long& userTime, long& kernelTime)
{
	userTime   = 0;
	kernelTime = 0;
}


ProcessHandleImpl* ProcessImpl::launchImpl(const std::string& command, const ArgsImpl& args, const std::string& initialDirectory,Pipe* inPipe, Pipe* outPipe, Pipe* errPipe, const EnvImpl& env)
{
	//throw Poco::NotImplementedException("Process::launch()");
	return 0;
}


void ProcessImpl::killImpl(ProcessHandleImpl& handle)
{
	//throw Poco::NotImplementedException("Process::kill()");
}


void ProcessImpl::killImpl(PIDImpl pid)
{
	//throw Poco::NotImplementedException("Process::kill()");
}


bool ProcessImpl::isRunningImpl(const ProcessHandleImpl& handle)
{
	//throw Poco::NotImplementedException("Process::is_running()");
	return false;
}


bool ProcessImpl::isRunningImpl(PIDImpl pid)
{
	//throw Poco::NotImplementedException("Process::is_running()");
	return false;
}


void ProcessImpl::requestTerminationImpl(PIDImpl pid)
{
	//throw Poco::NotImplementedException("Process::requestTermination()");
}


} // namespace Poco
