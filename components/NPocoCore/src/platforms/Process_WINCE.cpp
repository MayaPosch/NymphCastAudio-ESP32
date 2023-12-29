/*
	Process_WINCE.cpp 
*/


#include "Process_WINCE.h"
//#include "Poco/Exception.h"
#include "../NumberFormatter.h"
#include "../NamedEvent.h"
#include "../UnicodeConverter.h"
#include "../Pipe.h"


namespace Poco {


//
// ProcessHandleImpl
//
ProcessHandleImpl::ProcessHandleImpl(HANDLE hProcess, UInt32 pid):
	_hProcess(hProcess),
	_pid(pid)
{
}


ProcessHandleImpl::~ProcessHandleImpl()
{
	closeHandle();
}

void ProcessHandleImpl::closeHandle()
{
	if (_hProcess)
	{
		CloseHandle(_hProcess);
		_hProcess = NULL;
	}
}

UInt32 ProcessHandleImpl::id() const
{
	return _pid;
}


HANDLE ProcessHandleImpl::process() const
{
	return _hProcess;
}


int ProcessHandleImpl::wait() const
{
	DWORD rc = WaitForSingleObject(_hProcess, INFINITE);
	if (rc != WAIT_OBJECT_0) { return -1; }
		//throw SystemException("Wait failed for process", NumberFormatter::format(_pid));

	DWORD exitCode;
	if (GetExitCodeProcess(_hProcess, &exitCode) == 0) { return -1; }
		//throw SystemException("Cannot get exit code for process", NumberFormatter::format(_pid));

	return exitCode;
}


int ProcessHandleImpl::tryWait() const
{
	DWORD exitCode;
	if (GetExitCodeProcess(_hProcess, &exitCode) == 0) { return -1; }
		//throw SystemException("Cannot get exit code for process", NumberFormatter::format(_pid));
		
	if (exitCode == STILL_ACTIVE)
		return -1;
	else
		return exitCode;
}


//
// ProcessImpl
//
ProcessImpl::PIDImpl ProcessImpl::idImpl()
{
	return GetCurrentProcessId();
}


void ProcessImpl::timesImpl(long& userTime, long& kernelTime)
{
	FILETIME ftCreation;
	FILETIME ftExit;
	FILETIME ftKernel;
	FILETIME ftUser;

	if (GetThreadTimes(GetCurrentThread(), &ftCreation, &ftExit, &ftKernel, &ftUser) != 0)
	{
		ULARGE_INTEGER time;
		time.LowPart  = ftKernel.dwLowDateTime;
		time.HighPart = ftKernel.dwHighDateTime;
		kernelTime    = long(time.QuadPart/10000000L);
		time.LowPart  = ftUser.dwLowDateTime;
		time.HighPart = ftUser.dwHighDateTime;
		userTime      = long(time.QuadPart/10000000L);
	}
	else
	{
		userTime = kernelTime = -1;
	}
}


ProcessHandleImpl* ProcessImpl::launchImpl(const std::string& command, const ArgsImpl& args, const std::string& initialDirectory, Pipe* inPipe, Pipe* outPipe, Pipe* errPipe, const EnvImpl& env)
{
	std::wstring ucommand;
	UnicodeConverter::toUTF16(command, ucommand);

	std::string commandLine;
	for (ArgsImpl::const_iterator it = args.begin(); it != args.end(); ++it)
	{
		if (it != args.begin()) commandLine.append(" ");
		commandLine.append(*it);
	}

	std::wstring ucommandLine;
	UnicodeConverter::toUTF16(commandLine, ucommandLine);

	PROCESS_INFORMATION processInfo;
	BOOL rc = CreateProcessW(
		ucommand.c_str(),
		const_cast<wchar_t*>(ucommandLine.c_str()),
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		NULL/*&startupInfo*/,
		&processInfo
	);

	if (rc)
	{
		CloseHandle(processInfo.hThread);
		return new ProcessHandleImpl(processInfo.hProcess, processInfo.dwProcessId);
	}
	else { return 0; } // throw SystemException("Cannot launch process", command);
}


void ProcessImpl::killImpl(ProcessHandleImpl& handle)
{
	if (handle.process())
	{
		if (TerminateProcess(handle.process(), 0) == 0)
		{
			handle.closeHandle();
			//throw SystemException("cannot kill process");
			return; // TODO: report error.
		}
		handle.closeHandle();
	}
}


void ProcessImpl::killImpl(PIDImpl pid)
{
	HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	if (hProc)
	{
		if (TerminateProcess(hProc, 0) == 0)
		{
			CloseHandle(hProc);
			//throw SystemException("cannot kill process");
			return; // TODO: report error.
		}
		CloseHandle(hProc);
	}
	else
	{
		switch (GetLastError())
		{
		case ERROR_ACCESS_DENIED:
			//throw NoPermissionException("cannot kill process");
			return; // TODO: report error.
		case ERROR_NOT_FOUND:
			//throw NotFoundException("cannot kill process");
			return; // TODO: report error.
		default:
			//throw SystemException("cannot kill process");
			return; // TODO: report error.
		}
	}
}


bool ProcessImpl::isRunningImpl(const ProcessHandleImpl& handle)
{
	bool result = true;
	DWORD exitCode;
	BOOL rc = GetExitCodeProcess(handle.process(), &exitCode);
	if (!rc || exitCode != STILL_ACTIVE) result = false;
	return result;
}


bool ProcessImpl::isRunningImpl(PIDImpl pid)
{
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	bool result = true;
	DWORD exitCode;
	BOOL rc = GetExitCodeProcess(hProc, &exitCode);
	if (!rc || exitCode != STILL_ACTIVE) result = false;
	return result;}


void ProcessImpl::requestTerminationImpl(PIDImpl pid)
{
	NamedEvent ev(terminationEventName(pid));
	ev.set();
}


std::string ProcessImpl::terminationEventName(PIDImpl pid)
{
	std::string evName("POCOTRM");
	NumberFormatter::appendHex(evName, pid, 8);
	return evName;
}


} // namespace Poco
