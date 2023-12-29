/*
	Process_WIN32U.h - Definition of the ProcessImpl class for WIN32.
*/


#ifndef Core_Process_WIN32U_INCLUDED
#define Core_Process_WIN32U_INCLUDED


#include "../Core.h"
#include "../RefCountedObject.h"
#include <vector>
#include <map>
#include "UnWindows.h"


namespace Poco {

class Foundation_API ProcessHandleImpl: public RefCountedObject {
public:
	ProcessHandleImpl(HANDLE _hProcess, uint32_t pid);
	~ProcessHandleImpl();

	uint32_t id() const;
	HANDLE process() const;
	int wait() const;
	int tryWait() const;
	void closeHandle();

private:
	HANDLE _hProcess;
	uint32_t _pid;

	ProcessHandleImpl(const ProcessHandleImpl&);
	ProcessHandleImpl& operator = (const ProcessHandleImpl&);
};


class Pipe;


class Foundation_API ProcessImpl {
public:
	typedef uint32_t PIDImpl;
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
	static std::string terminationEventName(PIDImpl pid);
	static bool mustEscapeArg(const std::string& arg);
	static std::string escapeArg(const std::string& arg);
};


} // namespace Poco


#endif // Core_Process_WIN32U_INCLUDED
