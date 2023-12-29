/*
	Process_FreeRTOS.h - Definition of the ProcessImpl class for FreeRTOS.
*/


#ifndef Core_Process_FreeRTOS_INCLUDED
#define Core_Process_FreeRTOS_INCLUDED


#include "../Core.h"
#include "../RefCountedObject.h"
//#include <unistd.h>
#include <vector>
#include <map>


namespace Poco {


class Pipe;


class Foundation_API ProcessHandleImpl: public RefCountedObject
{
public:
	ProcessHandleImpl(int pid);
	~ProcessHandleImpl();

	int id() const;
	int wait() const;
	int tryWait() const;

private:
	//pid_t _pid;
	int _pid;
};


class Foundation_API ProcessImpl
{
public:
	//typedef pid_t PIDImpl;
	typedef int PIDImpl;
	typedef std::vector<std::string> ArgsImpl;
	typedef std::map<std::string, std::string> EnvImpl;

	static PIDImpl idImpl();
	static void timesImpl(long& userTime, long& kernelTime);
	static ProcessHandleImpl* launchImpl(
		const std::string& command,
		const ArgsImpl& args,
		const std::string& initialDirectory,
		Pipe* inPipe,
		Pipe* outPipe,
		Pipe* errPipe,
		const EnvImpl& env);
	static void killImpl(ProcessHandleImpl& handle);
	static void killImpl(PIDImpl pid);
	static bool isRunningImpl(const ProcessHandleImpl& handle);
	static bool isRunningImpl(PIDImpl pid);
	static void requestTerminationImpl(PIDImpl pid);

private:
	static ProcessHandleImpl* launchByForkExecImpl(
		const std::string& command,
		const ArgsImpl& args,
		const std::string& initialDirectory,
		Pipe* inPipe,
		Pipe* outPipe,
		Pipe* errPipe,
		const EnvImpl& env);
};


} // namespace Poco


#endif // Core_Process_FreeRTOS_INCLUDED
