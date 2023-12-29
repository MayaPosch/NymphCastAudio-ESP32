/*
	Path_UNIX.h - Definition of the PathImpl class for Unix.
*/


#ifndef Core_Path_UNIX_INCLUDED
#define Core_Path_UNIX_INCLUDED


#include "../Core.h"
#include <vector>


namespace Poco {


class PathImpl
{
public:
	static std::string currentImpl();
	static std::string homeImpl();
	static std::string configHomeImpl();
	static std::string dataHomeImpl();
	static std::string tempHomeImpl();
	static std::string cacheHomeImpl();
	static std::string tempImpl();
	static std::string configImpl();
	static std::string nullImpl();
	static std::string expandImpl(const std::string& path);
	static void listRootsImpl(std::vector<std::string>& roots);
};


} // namespace Poco


#endif // Foundation_Path_UNIX_INCLUDED
