/*
	Path_WINCE.h - Definition of the PathImpl class for WIN32.
*/


#ifndef Core_Path_WINCE_INCLUDED
#define Core_Path_WINCE_INCLUDED


#include "../Core.h"
#include <vector>


namespace Poco {


class Foundation_API PathImpl
{
public:
	static std::string currentImpl();
	static std::string homeImpl();
	static std::string configHomeImpl();
	static std::string dataHomeImpl();
	static std::string cacheHomeImpl();
	static std::string tempHomeImpl();
	static std::string tempImpl();
	static std::string configImpl();
	static std::string nullImpl();
	static std::string systemImpl();
	static std::string expandImpl(const std::string& path);
	static void listRootsImpl(std::vector<std::string>& roots);
	
	enum
	{
		MAX_PATH_LEN = 32767
	};
};


} // namespace Poco


#endif // Core_Path_WINCE_INCLUDED
