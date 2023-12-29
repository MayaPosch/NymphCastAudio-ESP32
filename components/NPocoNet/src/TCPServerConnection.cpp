/*
	TCPServerConnection.cpp
*/


#include "TCPServerConnection.h"
//#include "Poco/Exception.h"
//#include "Poco/ErrorHandler.h"


//using Poco::Exception;
//using Poco::ErrorHandler;


namespace Poco {
namespace Net {


TCPServerConnection::TCPServerConnection(const StreamSocket& socket):
	_socket(socket)
{
}


TCPServerConnection::~TCPServerConnection()
{
}


void TCPServerConnection::start() {
	//try
	//{
		run();
	/*}
	catch (Exception& exc)
	{
		ErrorHandler::handle(exc);
	}
	catch (std::exception& exc)
	{
		ErrorHandler::handle(exc);
	}
	catch (...)
	{
		ErrorHandler::handle();
	}*/
}


} } // namespace Poco::Net
