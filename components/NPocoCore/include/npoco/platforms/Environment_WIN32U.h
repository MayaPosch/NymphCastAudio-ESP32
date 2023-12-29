/*
	Environment_WIN32U.h - Definition of the EnvironmentImpl class for WIN32.
*/


#ifndef Core_Environment_WIN32U_INCLUDED
#define Core_Environment_WIN32U_INCLUDED


#include "../Core.h"


namespace Poco {


class Foundation_API EnvironmentImpl
{
public:
	typedef UInt8 NodeId[6]; /// Ethernet address.

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
};


} // namespace Poco


#endif // Core_Environment_WIN32U_INCLUDED
