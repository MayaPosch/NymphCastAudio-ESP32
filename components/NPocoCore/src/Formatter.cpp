/*
	Formatter.cpp
*/


#include "Formatter.h"
//#include "Poco/Exception.h"


namespace Poco {


Formatter::Formatter()
{
}


Formatter::~Formatter()
{
}


bool Formatter::setProperty(const std::string& /*name*/, const std::string& /*value*/) {
	//throw PropertyNotSupportedException();
	return false;
}


std::string Formatter::getProperty(const std::string& /*name*/) const {
	//throw PropertyNotSupportedException();
	return std::string();
}


} // namespace Poco
