/*
	Channel.cpp -
*/


#include "Channel.h"


namespace Poco {


Channel::Channel()
{
}


Channel::~Channel()
{
}


void Channel::open()
{
}


void Channel::close()
{
}


bool Channel::setProperty(const std::string& name, const std::string& /*value*/) {
	//throw PropertyNotSupportedException(name);
	return false;
}


std::string Channel::getProperty(const std::string& name) const {
	//throw PropertyNotSupportedException(name);
	return std::string();
}


} // namespace Poco
