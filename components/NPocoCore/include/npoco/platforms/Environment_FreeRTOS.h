/*
	Environment_FreeRTOS.h - Definition of the EnvironmentImpl class for FreeRTOS.
*/


#ifndef Core_Environment_FreeRTOS_INCLUDED
#define Core_Environment_FreeRTOS_INCLUDED


#include "../Core.h"
#include "../Mutex.h"
#include <map>


namespace Poco {


class Foundation_API EnvironmentImpl
{
public:
	typedef uint8_t NodeId[6]; /// Ethernet address.

	static std::string getImpl(const std::string& name);	
	static bool hasImpl(const std::string& name);	
	static bool setImpl(const std::string& name, const std::string& value);
	static std::string osNameImpl();
	static std::string osDisplayNameImpl();
	static std::string osVersionImpl();
	static std::string osArchitectureImpl();
	static std::string nodeNameImpl();
	static bool nodeIdImpl(NodeId& id);
	static unsigned processorCountImpl();

private:
	typedef std::map<std::string, std::string> StringMap;
	
	static StringMap _map;
	static FastMutex _mutex;
};


} // namespace Poco


#endif // Core_Environment_FreeRTOS_INCLUDED
