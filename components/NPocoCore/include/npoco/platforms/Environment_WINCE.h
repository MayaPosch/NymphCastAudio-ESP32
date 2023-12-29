/*
	Environment_WINCE.h - Definition of the EnvironmentImpl class for WINCE.
*/


#ifndef Core_Environment_WINCE_INCLUDED
#define Core_Environment_WINCE_INCLUDED


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
	
private:
	static bool envVar(const std::string& name, std::string* value);
	
	static const std::string TEMP;
	static const std::string TMP;
	static const std::string HOMEPATH;
	static const std::string COMPUTERNAME;
	static const std::string OS;
	static const std::string NUMBER_OF_PROCESSORS;
	static const std::string PROCESSOR_ARCHITECTURE;	
};


} // namespace Poco


#endif // Core_Environment_WINCE_INCLUDED
