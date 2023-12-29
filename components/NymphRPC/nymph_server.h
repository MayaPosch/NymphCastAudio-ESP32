/*
	nymph_server.h	- header file for the NymphRPC Server class.
	
	Revision 0
	
	Notes:
			- This class declares the server class to be used by Nymph servers.
			
	History:
	2017/06/24, Maya Posch : Initial version.
	
	(c) Nyanko.ws
*/


#pragma once
#ifndef NYMPH_SERVER_H
#define NYMPH_SERVER_H

#include <string>
#include <atomic>

#ifdef NPOCO
#include <npoco/net/TCPServer.h>
#else
#include <Poco/Net/TCPServer.h>
#endif


class NymphServer {
	static std::string loggerName;
	static Poco::Net::ServerSocket ss;
	static Poco::Net::TCPServer* server;
	static Poco::ThreadPool* threadpool;
	
public:
	static std::atomic<bool> running;
	
	static bool start(int port = 4004);
	static bool stop();
};

#endif
