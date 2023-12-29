/*
	Process_WIN32U.cpp
*/


#include "Process_WIN32U.h"
//#include "Poco/Exception.h"
#include "../NumberFormatter.h"
#include "../NamedEvent.h"
#include "../UnicodeConverter.h"
#include "../Pipe.h"
#include "../File.h"
#include "../Path.h"
#include "../NPString.h"
#include "../Ascii.h"


namespace 
{
	std::vector<wchar_t> getUnicodeEnvironmentVariablesBuffer(const Poco::Process::Env& env)
	{
		std::vector<wchar_t> envbuf;
		std::size_t pos = 0;

		for (const auto& p: env)
		{
			std::size_t envlen = p.first.length() + p.second.length() + 1;

			std::wstring uname;
			Poco::UnicodeConverter::convert(p.first, uname);
			std::wstring uvalue;
			Poco::UnicodeConverter::convert(p.second, uvalue);

			envbuf.resize(pos + envlen + 1);
			std::copy(uname.begin(), uname.end(), &envbuf[pos]);
			pos += uname.length();
			envbuf[pos] = L'=';
			++pos;
			std::copy(uvalue.begin(), uvalue.end(), &envbuf[pos]);
			pos += uvalue.length();

			envbuf[pos] = L'\0';
			++pos;
		}

		envbuf.resize(pos + 1);
		envbuf[pos] = L'\0';

		return envbuf;
	}
}


namespace Poco {


//
// ProcessHandleImpl
//
ProcessHandleImpl::ProcessHandleImpl(HANDLE hProcess, uint32_t pid) :
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

	if (GetProcessTimes(GetCurrentProcess(), &ftCreation, &ftExit, &ftKernel, &ftUser) != 0)
	{
		ULARGE_INTEGER time;
		time.LowPart = ftKernel.dwLowDateTime;
		time.HighPart = ftKernel.dwHighDateTime;
		kernelTime = long(time.QuadPart / 10000000L);
		time.LowPart = ftUser.dwLowDateTime;
		time.HighPart = ftUser.dwHighDateTime;
		userTime = long(time.QuadPart / 10000000L);
	}
	else
	{
		userTime = kernelTime = -1;
	}
}


bool ProcessImpl::mustEscapeArg(const std::string& arg)
{
	bool result = false;
	bool inQuotes = false;
	bool escaped = false;
	for (char c: arg)
	{
		if (Poco::Ascii::isSpace(c) && !inQuotes && !escaped)
		{
			result = true;
			break;
		}
		else if (c == '"' && !escaped)
		{
			inQuotes = !inQuotes;
		}
		else if (c == '\\' && !escaped)
		{
			escaped = true;
		}
		else
		{
			escaped = false;
		}
	}
	return result || inQuotes;
}


// Based on code from https://blogs.msdn.microsoft.com/twistylittlepassagesallalike/2011/04/23/everyone-quotes-command-line-arguments-the-wrong-way/
std::string ProcessImpl::escapeArg(const std::string& arg)
{
	if (mustEscapeArg(arg))
	{
		std::string quotedArg("\"");
		for (std::string::const_iterator it = arg.begin(); ; ++it)
		{
			unsigned backslashCount = 0;
			while (it != arg.end() && '\\' == *it)
			{
				++it;
				++backslashCount;
			}

			if (it == arg.end())
			{
				quotedArg.append(2 * backslashCount, '\\');
				break;
			}
			else if ('"' == *it)
			{
				quotedArg.append(2 * backslashCount + 1, '\\');
				quotedArg.push_back('"');
			}
			else
			{
				quotedArg.append(backslashCount, '\\');
				quotedArg.push_back(*it);
			}
		}
		quotedArg.push_back('"');
		return quotedArg;
	}
	else return arg;
}


ProcessHandleImpl* ProcessImpl::launchImpl(const std::string& command, const ArgsImpl& args, const std::string& initialDirectory, Pipe* inPipe, Pipe* outPipe, Pipe* errPipe, const EnvImpl& env)
{
	std::string commandLine = escapeArg(command);
	for (const auto& a: args)
	{
		commandLine.append(" ");
		commandLine.append(escapeArg(a));
	}

	std::wstring ucommandLine;
	UnicodeConverter::toUTF16(commandLine, ucommandLine);

	const wchar_t* applicationName = 0;
	std::wstring uapplicationName;
	if (command.size() > MAX_PATH)
	{
		Poco::Path p(command);
		if (p.isAbsolute())
		{
			UnicodeConverter::toUTF16(command, uapplicationName);
			if (p.getExtension().empty()) uapplicationName += L".EXE";
			applicationName = uapplicationName.c_str();
		}
	}

	STARTUPINFOW startupInfo;
	GetStartupInfoW(&startupInfo); // take defaults from current process
	startupInfo.cb = sizeof(STARTUPINFOW);
	startupInfo.lpReserved = NULL;
	startupInfo.lpDesktop = NULL;
	startupInfo.lpTitle = NULL;
	startupInfo.dwFlags = STARTF_FORCEOFFFEEDBACK;
	startupInfo.cbReserved2 = 0;
	startupInfo.lpReserved2 = NULL;

	HANDLE hProc = GetCurrentProcess();
	bool mustInheritHandles = false;
	if (inPipe)
	{
		DuplicateHandle(hProc, inPipe->readHandle(), hProc, &startupInfo.hStdInput, 0, TRUE, DUPLICATE_SAME_ACCESS);
		mustInheritHandles = true;
		inPipe->close(Pipe::CLOSE_READ);
	}
	else if (GetStdHandle(STD_INPUT_HANDLE))
	{
		DuplicateHandle(hProc, GetStdHandle(STD_INPUT_HANDLE), hProc, &startupInfo.hStdInput, 0, TRUE, DUPLICATE_SAME_ACCESS);
		mustInheritHandles = true;
	}
	else
	{
		startupInfo.hStdInput = 0;
	}
	// outPipe may be the same as errPipe, so we duplicate first and close later.
	if (outPipe)
	{
		DuplicateHandle(hProc, outPipe->writeHandle(), hProc, &startupInfo.hStdOutput, 0, TRUE, DUPLICATE_SAME_ACCESS);
		mustInheritHandles = true;
	}
	else if (GetStdHandle(STD_OUTPUT_HANDLE))
	{
		DuplicateHandle(hProc, GetStdHandle(STD_OUTPUT_HANDLE), hProc, &startupInfo.hStdOutput, 0, TRUE, DUPLICATE_SAME_ACCESS);
		mustInheritHandles = true;
	}
	else
	{
		startupInfo.hStdOutput = 0;
	}
	if (errPipe)
	{
		DuplicateHandle(hProc, errPipe->writeHandle(), hProc, &startupInfo.hStdError, 0, TRUE, DUPLICATE_SAME_ACCESS);
		mustInheritHandles = true;
	}
	else if (GetStdHandle(STD_ERROR_HANDLE))
	{
		DuplicateHandle(hProc, GetStdHandle(STD_ERROR_HANDLE), hProc, &startupInfo.hStdError, 0, TRUE, DUPLICATE_SAME_ACCESS);
		mustInheritHandles = true;
	}
	else
	{
		startupInfo.hStdError = 0;
	}
	if (outPipe) outPipe->close(Pipe::CLOSE_WRITE);
	if (errPipe) errPipe->close(Pipe::CLOSE_WRITE);

	if (mustInheritHandles)
	{
		startupInfo.dwFlags |= STARTF_USESTDHANDLES;
	}

	std::wstring uinitialDirectory;
	UnicodeConverter::toUTF16(initialDirectory, uinitialDirectory);
	const wchar_t* workingDirectory = uinitialDirectory.empty() ? 0 : uinitialDirectory.c_str();

	const wchar_t* pEnv = 0;
	std::vector<wchar_t> envChars;
	if (!env.empty())
	{
		envChars = getUnicodeEnvironmentVariablesBuffer(env);
		pEnv = &envChars[0];
	}

	PROCESS_INFORMATION processInfo;
	DWORD creationFlags = GetConsoleWindow() ? 0 : CREATE_NO_WINDOW;
	if (pEnv) creationFlags |= CREATE_UNICODE_ENVIRONMENT;
	BOOL rc = CreateProcessW(
		applicationName,
		const_cast<wchar_t*>(ucommandLine.c_str()),
		NULL, // processAttributes
		NULL, // threadAttributes
		mustInheritHandles,
		creationFlags,
		(LPVOID)pEnv,
		workingDirectory,
		&startupInfo,
		&processInfo
	);
	if (startupInfo.hStdInput) CloseHandle(startupInfo.hStdInput);
	if (startupInfo.hStdOutput) CloseHandle(startupInfo.hStdOutput);
	if (startupInfo.hStdError) CloseHandle(startupInfo.hStdError);
	if (rc)
	{
		CloseHandle(processInfo.hThread);
		return new ProcessHandleImpl(processInfo.hProcess, processInfo.dwProcessId);
	}
	else { return 0; } //throw SystemException("Cannot launch process", command);
}


void ProcessImpl::killImpl(ProcessHandleImpl& handle)
{
	if (handle.process())
	{
		if (TerminateProcess(handle.process(), 0) == 0)
		{
			handle.closeHandle();
			//throw SystemException("cannot kill process");
			return;
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
			return;
		}
		CloseHandle(hProc);
	}
	else
	{
		switch (GetLastError())
		{
		case ERROR_ACCESS_DENIED:
			//throw NoPermissionException("cannot kill process");
			return;
		case ERROR_NOT_FOUND:
			//throw NotFoundException("cannot kill process");
			return;
		case ERROR_INVALID_PARAMETER:
			//throw NotFoundException("cannot kill process");
			return;
		default:
			//throw SystemException("cannot kill process");
			return;
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
	return result;
}


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
