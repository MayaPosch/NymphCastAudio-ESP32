/*
	SocketAddressImpl.cpp 
*/


#include "SocketAddressImpl.h"
#include "SocketDefs.h"
#include "npoco/NumberFormatter.h"
#include <cstring>

// debug
#include <iostream>
#include <string>


namespace Poco {
namespace Net {
namespace Impl {


//
// SocketAddressImpl
//


SocketAddressImpl::SocketAddressImpl()
{
}


SocketAddressImpl::~SocketAddressImpl()
{
}


//
// IPv4SocketAddressImpl
//


IPv4SocketAddressImpl::IPv4SocketAddressImpl()
{
	std::memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	poco_set_sin_len(&_addr);
}


IPv4SocketAddressImpl::IPv4SocketAddressImpl(const struct sockaddr_in* addr)
{
	std::memcpy(&_addr, addr, sizeof(_addr));
}


IPv4SocketAddressImpl::IPv4SocketAddressImpl(const void* addr, UInt16 port)
{
	// debug
	std::cout << "IPv4SocketAddressImpl constructor. Port: " << port << std::endl;
	/* std::cout << "Socket address: ";
	std::cout << std::ios::hex;
    for (int32_t i = 0; i < 16; ++i) { 
        std::cout << (uint16_t) (((sockaddr*) addr)->sa_data)[i] << " ";
    }
    
    std::cout << std::ios::dec << std::endl; */
	
	std::memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	poco_set_sin_len(&_addr);
	std::memcpy(&_addr.sin_addr, addr, sizeof(_addr.sin_addr));
	_addr.sin_port = port;
	
	/* _addr.sin_family = AF_INET;
	_addr.sin_port = port;
	_addr.sin_addr = *((in_addr*) addr) */;
	//_addr.sin_len = sizeof(struct sockaddr_in);
	//_addr.sin_zero
	
	// debug
	std::cout << "Assigned new IPv4 address. Port: " << _addr.sin_port 
				<< ", family: " << _addr.sin_family << std::endl;
}


std::string IPv4SocketAddressImpl::toString() const
{
	std::string result;
	result.append(host().toString());
	result.append(":");
	NumberFormatter::append(result, ntohs(port()));
	return result;
}


#if defined(POCO_HAVE_IPv6)


//
// IPv6SocketAddressImpl
//


IPv6SocketAddressImpl::IPv6SocketAddressImpl(const struct sockaddr_in6* addr)
{
	std::memcpy(&_addr, addr, sizeof(_addr));
}


IPv6SocketAddressImpl::IPv6SocketAddressImpl(const void* addr, UInt16 port)
{
	std::memset(&_addr, 0, sizeof(_addr));
	_addr.sin6_family = AF_INET6;
	poco_set_sin6_len(&_addr);
	std::memcpy(&_addr.sin6_addr, addr, sizeof(_addr.sin6_addr));
	_addr.sin6_port = port;
}


IPv6SocketAddressImpl::IPv6SocketAddressImpl(const void* addr, UInt16 port, UInt32 scope)
{
	std::memset(&_addr, 0, sizeof(_addr));
	_addr.sin6_family = AF_INET6;
	poco_set_sin6_len(&_addr);
	std::memcpy(&_addr.sin6_addr, addr, sizeof(_addr.sin6_addr));
	_addr.sin6_port = port;
	_addr.sin6_scope_id = scope;
}


std::string IPv6SocketAddressImpl::toString() const
{
	std::string result;
	result.append("[");
	result.append(host().toString());
	result.append("]");
	result.append(":");
	NumberFormatter::append(result, ntohs(port()));
	return result;
}


#endif // POCO_HAVE_IPv6


#if defined(POCO_OS_FAMILY_UNIX)


//
// LocalSocketAddressImpl
//


LocalSocketAddressImpl::LocalSocketAddressImpl(const struct sockaddr_un* addr)
{
	_pAddr = new sockaddr_un;
	std::memcpy(_pAddr, addr, sizeof(struct sockaddr_un));
}


LocalSocketAddressImpl::LocalSocketAddressImpl(const char* path)
{
	poco_assert (std::strlen(path) < sizeof(_pAddr->sun_path));

	_pAddr = new sockaddr_un;
	poco_set_sun_len(_pAddr, std::strlen(path) + sizeof(struct sockaddr_un) - sizeof(_pAddr->sun_path) + 1);
	_pAddr->sun_family = AF_UNIX;
	std::strcpy(_pAddr->sun_path, path);
}


LocalSocketAddressImpl::LocalSocketAddressImpl(const char* path, std::size_t length)
{
	poco_assert (length < sizeof(_pAddr->sun_path));

	_pAddr = new sockaddr_un;
	poco_set_sun_len(_pAddr, length + sizeof(struct sockaddr_un) - sizeof(_pAddr->sun_path) + 1);
	_pAddr->sun_family = AF_UNIX;
	std::memcpy(_pAddr->sun_path, path, length);
	_pAddr->sun_path[length] = 0;
}


LocalSocketAddressImpl::~LocalSocketAddressImpl()
{
	delete _pAddr;
}


std::string LocalSocketAddressImpl::toString() const
{
	std::string result(path());
	return result;
}


#endif // POCO_OS_FAMILY_UNIX


} } } // namespace Poco::Net::Impl
