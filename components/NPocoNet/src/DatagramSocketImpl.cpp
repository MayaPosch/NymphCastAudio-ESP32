/*
	DatagramSocketImpl.cpp
*/


#include "DatagramSocketImpl.h"
//#include "NetException.h"


//using Poco::InvalidArgumentException;


namespace Poco {
namespace Net {


DatagramSocketImpl::DatagramSocketImpl()
{
}


DatagramSocketImpl::DatagramSocketImpl(SocketAddress::Family family)
{
	if (family == SocketAddress::IPv4)
		initSocket(AF_INET, SOCK_DGRAM);
#if defined(POCO_HAVE_IPv6)
	else if (family == SocketAddress::IPv6)
		initSocket(AF_INET6, SOCK_DGRAM);
#endif
#if defined(POCO_OS_FAMILY_UNIX)
	else if (family == SocketAddress::UNIX_LOCAL)
		initSocket(AF_UNIX, SOCK_DGRAM);
#endif
	//else throw InvalidArgumentException("Invalid or unsupported address family passed to DatagramSocketImpl");
	// TODO: handle error.
}


DatagramSocketImpl::DatagramSocketImpl(poco_socket_t sockfd): SocketImpl(sockfd)
{
}


DatagramSocketImpl::~DatagramSocketImpl()
{
}


bool DatagramSocketImpl::init(int af)
{
	initSocket(af, SOCK_DGRAM);
	return true;
}


} } // namespace Poco::Net
