/*
	nyansd.cpp - Implementation file for the NyanSD service discovery library.
	
	Notes:
			- 
			
	2020/04/23, Maya Posch
*/


// Uncomment or define DEBUG to enable debug output.
#define DEBUG 1


#include "nyansd.h"

//#include <iostream>
#include <map>

#ifdef NPOCO
	#include <npoco/net/DatagramSocket.h>
#ifndef ESP_PLATFORM
	#include <npoco/net/NetworkInterface.h>
#else
	#include <esp_netif.h>
#endif
	#include <npoco/net/DNS.h>
#else
	#include <Poco/Net/DatagramSocket.h>
	#include <Poco/Net/NetworkInterface.h>
	#include <Poco/Net/DNS.h>
	#include <Poco/Net/NetException.h>
	#include <Poco/Exception.h>
#endif


// Static variables.
std::vector<NYSD_service> NyanSD::services;
std::mutex NyanSD::servicesMutex;
std::atomic<bool> NyanSD::running{false};
//std::thread NyanSD::handler;
//pthread_t NyanSD::handler;
Poco::Thread NyanSD::handler("nyansd");
//ByteBauble NyanSD::bb;


struct ResponseStruct {
	char* data;
	uint32_t length;
};


// --- SEND QUERY ---
bool NyanSD::sendQuery(uint16_t port, std::vector<NYSD_query> queries, 
									std::vector<NYSD_service> &responses) {
	if (queries.size() > 255) {
		//std::cerr << "No more than 255 queries can be send simultaneously." << std::endl;
		return false;
	}
	else if (queries.size() < 1) {
		//std::cerr << "At least one query must be sent. No query found." << std::endl;
		return false;
	}
				
	// Compose the NYSD message.
	//BBEndianness he = bb.getHostEndian();
	std::string msg = "NYANSD";
	uint16_t len = 0;
	uint8_t type = (uint8_t) NYSD_MESSAGE_TYPE_BROADCAST;
	
	std::string body;
	uint8_t qnum = queries.size();
	body += std::string((char*) &qnum, 1);
	for (int i = 0; i < qnum; ++i) {
		body += std::string("Q");
		uint8_t prot = (uint8_t) queries[i].protocol;
		uint8_t qlen = (uint8_t) queries[i].filter.length();
		body += (char) prot;
		body += (char) qlen;
		if (qlen > 0) {
			body += queries[i].filter;
		}
	}
	
	len = body.length() + 1;	// Add one byte for the message type.
	//len = bb.toGlobal(len, he);
	msg += std::string((char*) &len, 2);
	msg += (char) type;
	msg += body;
	
#ifdef DEBUG
	std::cout << "Message length: " << msg.length() << std::endl;
#endif
	
	std::vector<ResponseStruct> buffers;
#ifndef ESP_PLATFORM
	// Open UDP socket for each interface and send the broadcast message.
	std::map<uint32_t, Poco::Net::NetworkInterface> interfaces = Poco::Net::NetworkInterface::map(true, true);
	uint32_t ifc_size = interfaces.size();
	
#ifdef DEBUG
	std::cout << "Found " << ifc_size << " network interfaces." << std::endl;
#endif
	
	std::map<uint32_t, Poco::Net::NetworkInterface>::const_iterator it;
	for (it = interfaces.begin(); it != interfaces.end(); ++it) {
		const Poco::Net::NetworkInterface& ifc = it->second;
		
		std::cerr << "Network interface '" << ifc.displayName() << "'." << std::endl;
		
		if (!ifc.supportsIPv4()) {
			std::cerr << "Network interface " << it->first << " does not support IPv4." << std::endl;
			continue; 
		}
		
		Poco::Net::IPAddress ip;
#ifndef NPOCO
		try {
#endif
			ip = ifc.firstAddress(Poco::Net::IPAddress::IPv4);
#ifndef NPOCO
		}
		catch (Poco::NotFoundException &e) {
			std::cerr << "Received NotFoundException: " << e.displayText() << std::endl;
			continue;
		}
		catch (...) {
			std::cerr << "Received unknown exception." << std::endl;
			continue;
		}
#endif
		
		if (!ip.isIPv4Compatible()) {
			std::cerr << "Not an IPv4 IP, skipping." << std::endl;
			continue;
		}
		
		std::string ipStr = ip.toString();
		
#ifdef DEBUG
		std::cout << "Modifying IP address: " << ipStr << std::endl;
#endif

#else
		// ESP-IDF: get the local IP address.
		// Copy the netif IP info into our variable.
		esp_netif_ip_info_t ip_info;
		esp_netif_t* netif = 0;
        netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
		esp_netif_get_ip_info(netif, &ip_info);
		std::string ipStr = ipv4_uintToString(ip_info.ip.addr);
#endif // ESP_PLATFORM
		
		// Replace the last digits with '255' to make it into a broadcast address.
		ipStr.replace(ipStr.find_last_of('.') + 1, 3, "255");
		
#ifdef DEBUG
		std::cout << "Broadcast IP address: " << ipStr << std::endl;
#endif
		
		Poco::Net::DatagramSocket udpsocket(Poco::Net::IPAddress::IPv4);
		udpsocket.setBroadcast(true);
		Poco::Net::SocketAddress sa(ipStr, port);
		
#ifdef DEBUG
		std::cout << "Sending..." << std::endl;
#endif
		
#ifndef NPOCO
		try {
#endif
			udpsocket.sendTo(msg.data(), msg.length(), sa);
#ifndef NPOCO
		}
		catch (Poco::Net::NetException &e) {
			std::cerr << "UDP Socket sendTo: got exception - " << e.displayText() << std::endl;
			continue;
		}
		catch (...) {
			std::cerr << "UDP Socket sendTo: got unknown exception." << std::endl;
			continue;
		}
#endif
		
#ifdef DEBUG
		std::cout << "Listening..." << std::endl;
#endif
		
		// Listen for responses for 500 milliseconds.
		Poco::Timespan ts(500000);	// 500 ms timeout.
		int n;
		Poco::Net::Socket::SocketList readList, writeList, exceptList;
		readList.push_back(udpsocket);
		while (Poco::Net::Socket::select(readList, writeList, exceptList, ts)) {
			ResponseStruct rs;
			rs.data = new char[2048];
#ifndef NPOCO
			try {
#endif
				rs.length = udpsocket.receiveBytes(rs.data, 2048, 0);
#ifndef NPOCO
			}
			catch (Poco::TimeoutException &exc) {
				std::cerr << "ReceiveBytes: " << exc.displayText() << std::endl;
				udpsocket.close();
				continue;
			}
			catch (...) {
				std::cerr << "ReceiveBytes: Unknown exception." << std::endl;
				continue;
			}
#endif
			
#ifdef DEBUG
			std::cout << "Received message with length " << rs.length << std::endl;
#endif
			
			buffers.push_back(rs);
#ifndef ESP_PLATFORM
		}
#endif
		
		// Close socket as we're done with this interface.
		udpsocket.close();
	}
	
#ifdef DEBUG
	std::cout << "Parsing " << buffers.size() << " response(s)..." << std::endl;
#endif
	
	// Copy parsed responses into the 'responses' vector.
	for (int i = 0; i < buffers.size(); ++i) {
		int n = buffers[i].length;
		if (n < 8) {
			// Nothing to do.	
#ifdef DEBUG
			std::cout << "No responses were received." << std::endl;
#endif
			return false;
		}
		
		// The received data can contain more than one response. Start parsing from the beginning until
		// we are done.
		char* buffer = buffers[i].data;
		int index = 0;
		while (index < n) {
			std::string signature = std::string(buffer, 6);
			index += 6;
			if (signature != "NYANSD") {
				std::cerr << "Signature of message incorrect: " << signature << std::endl;
				return false;
			}
			
			len = *((uint16_t*) &buffer[index]);
			//len = bb.toHost(len, BB_LE);
			index += 2;
			
			if (len > buffers[i].length - (index)) {
				std::cerr << "Insufficient data in buffer to finish parsing message: " << len << "/" 
							<< (buffers[i].length - (index + 6)) << std::endl;
				return false;
			}
			
#ifdef DEBUG
			std::cout << "Found message with length: " << len << std::endl;
#endif
			
			type = *((uint8_t*) &buffer[index++]);
			
#ifdef DEBUG
			std::cout << "Message type: " << (uint16_t) type << std::endl;
#endif
			
			if (type != NYSD_MESSAGE_TYPE_RESPONSE) {
				std::cerr << "Not a response message type. Skipping..." << std::endl;
				continue;
			}
			
			uint8_t rnum = *((char*) &buffer[index++]);		
#ifdef DEBUG
			std::cout << "Response count: " << (uint16_t) rnum << std::endl;
#endif
			
			// Service sections.
			for (int i = 0; i < rnum; ++i) {
				if ((buffer[index] != 'S') != 0) {
					//std::cerr << "Invalid service section signature. Aborting parsing." << std::endl;
					return false;
				}
				
				index++;
				uint32_t ipv4 = *((uint32_t*) &buffer[index]);
				//ipv4 = bb.toHost(ipv4, BB_LE);
				index += 4;
				
				uint8_t ipv6len = *((uint8_t*) &buffer[index++]);
				
#ifdef DEBUG
				std::cout << "IPv6 string with length: " << (uint16_t) ipv6len << std::endl;
#endif
				
				std::string ipv6 = std::string(buffer + index, buffer + (index + ipv6len));
				index += ipv6len;
				
				uint16_t hostlen = *((uint16_t*) &buffer[index]);
				//hostlen = bb.toHost(hostlen, BB_LE);
				index += 2;
				
				std::string hostname = std::string(buffer + index, buffer + (index + hostlen));
				index += hostlen;
				
				uint16_t port = *((uint16_t*) &buffer[index]);
				//port = bb.toHost(port, BB_LE);
				index += 2;
				
				uint8_t prot = *((uint8_t*) &buffer[index++]);
				
				uint16_t snlen = *((uint16_t*) &buffer[index]);
				//snlen = bb.toHost(snlen, BB_LE);
				index += 2;
				
				std::string svname = std::string(buffer + index, buffer + (index + snlen));
				index += snlen;
				
#ifdef DEBUG
				std::cout << "Adding service with name: " << svname << std::endl;
#endif
				
				NYSD_service sv;
				sv.ipv4 = ipv4;
				sv.ipv6 = ipv6;
				sv.port = port;
				sv.hostname = hostname;
				sv.service = svname;
				if (prot == NYSD_PROTOCOL_ALL) {
					sv.protocol = NYSD_PROTOCOL_ALL;
				}
				else if (prot == NYSD_PROTOCOL_TCP) {
					sv.protocol = NYSD_PROTOCOL_TCP;
				}
				else if (prot == NYSD_PROTOCOL_UDP) {
					sv.protocol = NYSD_PROTOCOL_UDP;
				}
				
				responses.push_back(sv);
			}
			
#ifdef DEBUG
			std::cout << "Buffer: " << index << "/" << n << std::endl;
#endif
		}
			
		delete[] buffers[i].data;
	}
	
	return true;
}


// --- ADD SERVICE ---
// Adds a service entry to the list. Only the protocol, port and service entries are required, 
// the others will be filled in if left empty.
bool NyanSD::addService(NYSD_service service) {
	if (service.port == 0 || service.service.empty()) {
		std::cerr << "Invalid service entry: " << service.service << ":" << service.port << std::endl;
		return false;
	}
	
	if (service.hostname.empty()) {
		// Fill in the hostname of the system.
		service.hostname = Poco::Net::DNS::hostName();
	}
	
	servicesMutex.lock();
	services.push_back(service);
	servicesMutex.unlock();
	
	return true;
}


// --- START LISTENER ---
bool NyanSD::startListener(uint16_t port) {
	if (running) {
		std::cerr << "Client handler thread is already running." << std::endl;
		return false;
	}
	
	// Create new thread with the client handler.
	//handler = std::thread(&NyanSD::clientHandler, port);
	/* pthread_attr_t attr;
	ssize_t stack_size;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 3584); // 3.5 kB
	int err = pthread_create(&handler, &attr, &NyanSD::clientHandler, (void*) &port); */
#ifdef ESP_PLATFORM
	//handler.setStackSize(12288); // 12 kB.
	//handler.setStackSize(18432); // 18 kB.
	handler.setStackSize(20480); // 20 kB.
	handler.useExternalRAM(true); // Use external RAM for stack.
#endif
	handler.start(NyanSD::clientHandler, (void*) &port);
	
	return true;
}


// --- STOP LISTENER ---
bool NyanSD::stopListener() {
	// Stop the listening socket and clean-up resources.
	running = false;
	//handler.join();
	//pthread_join(handler, NULL);
	handler.join();
	
	return true;
}


std::string NyanSD::ipv4_uintToString(uint32_t ipv4) {
	std::string out;
	for (int i = 0; i < 4; ++i) {
		out += std::to_string(*(((uint8_t*) &ipv4) + i));
		if (i < 3) { out += "."; }
	}
	
	return out;
}


uint32_t NyanSD::ipv4_stringToUint(std::string ipv4) {
	// String should have the format: DD.DD.DD.DD, where 'DD' is a value between 0-255.
#ifdef DEBUG
	std::cout << "IP to convert: " << ipv4 << std::endl;
#endif
	uint32_t out;
	uint8_t* op = (uint8_t*) &out;
	std::size_t pos = 0;
	std::size_t pos_end = 0;
	for (int i = 0; i < 4; i++) {
		pos_end = ipv4.find(".", pos + 1);
		*op = (uint8_t) std::stoul(ipv4.substr(pos, pos_end - pos));
		pos = pos_end;
		pos++;
		op++;
	}
	
#ifdef DEBUG
	std::cout << "Converted IP: " << std::showbase << std::hex << out << std::dec << std::endl;
#endif
	
	return out;
}


// --- REMOTE TO LOCAL IP ---
bool remoteToLocalIP(Poco::Net::SocketAddress &sa, uint32_t &ipv4, std::string &ipv6) {
#ifdef ESP_PLATFORM
	//
	esp_netif_ip_info_t ip_info;
	esp_netif_t* netif = 0;
	netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
	esp_netif_get_ip_info(netif, &ip_info);
	ipv4 = ip_info.ip.addr;
	
	return true;
#else
	std::map<unsigned, Poco::Net::NetworkInterface> map = Poco::Net::NetworkInterface::map(true, false);
	std::map<unsigned, Poco::Net::NetworkInterface>::const_iterator it = map.begin();
	std::map<unsigned, Poco::Net::NetworkInterface>::const_iterator end = map.end();

	bool isIPv6 = true;
	if (sa.family() == Poco::Net::IPAddress::IPv4) { isIPv6 = false; }
	
	std::string addr = sa.toString();
#ifdef DEBUG
	std::cout << "Sender was IP: " << addr << std::endl;
#endif
	if (isIPv6) {
		addr.erase(addr.find_last_of(':') + 1);
	}
	else {
		addr.erase(addr.find_last_of('.') + 1);
	}
	
#ifdef DEBUG
	std::cout << "LAN base IP: " << addr << std::endl;
#endif
	for (; it != end; ++it) {
		const std::size_t count = it->second.addressList().size();
		for (int i = 0; i < count; ++i) {
			std::string ip = it->second.address(i).toString();
			
#ifdef DEBUG
			std::cout << "Checking IP: " << ip << std::endl;
#endif
			if (addr.compare(0, addr.length(), ip, 0, addr.length()) == 0) {
#ifdef DEBUG
				std::cout << "Found IP: " << ip << std::endl;
#endif
				if (!isIPv6) {
					// Find first IPv4 address on this network interface.
					for (int j = 0; j < count; ++j) {
						if (it->second.address(j).af() == AF_INET) {
							ipv4 = NyanSD::ipv4_stringToUint(it->second.address(j).toString());
							return true;
						}
					}
				}
				else {
					// Find first IPv6 address on this network interface.
					for (int j = 0; j < count; ++j) {
						if (it->second.address(j).af() == AF_INET6) {
							ipv6 = it->second.address(j).toString();
							
							// Remove trailing '%<if>' section on certain OSes.
							std::string::size_type st = ipv6.find_last_of('%');
							if (st != std::string::npos) { ipv6.erase(st); }
					
							return true;
						}
					}
				}
				
				return false;
			}
		}
	}
	
#ifdef DEBUG
	std::cout << "LAN IP not found on interfaces." << std::endl;
#endif
	
	return false;
#endif // ESP_PLATFORM
}


// --- CLIENT HANDLER ---
//void NyanSD::clientHandler(uint16_t port) {
void NyanSD::clientHandler(void* arg) {
	uint16_t port = *((uint16_t*) arg);
	
	std::cout << "NyanSD: Start client handler..." << std::endl;
	
	// Set up listening socket on the provided port.
	Poco::Net::DatagramSocket udpsocket;
	
	std::cout << "NyanSD: Set broadcast..." << std::endl;
	
	udpsocket.setBroadcast(true);
	
	std::cout << "NyanSD: New SocketAddress..." << std::endl;
	
	Poco::Net::SocketAddress sa(Poco::Net::IPAddress(), port);
	
	std::cout << "NyanSD: UDP bind..." << std::endl;
	
	udpsocket.bind(sa, true);
	
	std::cout << "NyanSD: Start listening loop..." << std::endl;
	
	// Start listening loop.
	running = true;
	while (running) {
		// Read data in from socket.
		Poco::Net::SocketAddress sender;
		Poco::Timespan span(250000);
		if (udpsocket.poll(span, Poco::Net::Socket::SELECT_READ)) {
			char buffer[2048];
			int n = 0;
#ifndef NPOCO
			try {
#endif
				n = udpsocket.receiveFrom(buffer, sizeof(buffer), sender);
#ifndef NPOCO
			}
			catch (Poco::Exception &exc) {
				std::cerr << "ReceiveFrom: " << exc.displayText() << std::endl;
				continue;
			}
#endif
			
			if (n < 10) {
				//std::cerr << "Received incomplete message. Skipping." << std::endl;
				continue;
			}
			
#ifdef DEBUG
			std::cout << "Message length: " << n << std::endl;
#endif
			
			// Validate signature.
			int index = 0;
			std::string signature = std::string(buffer, 6);
			index += 6;
			if (signature != "NYANSD") {
				//std::cerr << "Signature of message incorrect: " << signature << std::endl;
				continue;
			}
			
			uint16_t len = *((uint16_t*) &buffer[index]);
			//len = bb.toHost(len, BB_LE);
			index += 2;
			
			if ((n - 8) != len) {
				//std::cerr << "Failed to read full message: " << n << "/" << len << std::endl;
				continue;
			}
		
			uint8_t type = *((uint8_t*) &buffer[index++]);
#ifdef DEBUG
			std::cout << "Message type: " << (uint16_t) type << std::endl;
#endif
			if (type != NYSD_MESSAGE_TYPE_BROADCAST) {
				//std::cerr << "Not a broadcast message type. Skipping..." << std::endl;
				continue;
			}
			
			// Parse message for queries.
			//BBEndianness he = bb.getHostEndian();
			uint8_t rnum = *((char*) &buffer[index++]);
#ifdef DEBUG
			std::cout << "Query count: " << (uint16_t) rnum << std::endl;
#endif
			
			if (rnum == 0) {
				//std::cerr << "Broadcast message didn't contain any queries. Skipping..." << std::endl;
				continue;
			}
		
			// Query sections.
			for (int i = 0; i < rnum; ++i) {
				if (buffer[index] != 'Q') {
					/*std::cerr << "Invalid query section signature: " << std::showbase << std::hex 
								<< (uint16_t) buffer[index] << std::dec
								<< ". Aborting parsing." << std::endl;*/
					continue;
				}
				
				index++;
				uint8_t prot = *((uint8_t*) &buffer[index++]);				
				uint8_t qlen = *((uint8_t*) &buffer[index++]);
				
				std::string servicesBody;
				uint8_t scount = 0;
				if (qlen != 0) {
					std::string filter = std::string(&(buffer[index]), qlen);
					index += qlen;
					
					// Create response body if request matches local data.
					// The match is done using a simple substring compare on the beginning of the
					// filter and service string.
					servicesMutex.lock();
					for (int i = 0; i < services.size(); ++i) {
						if (filter.compare(0, filter.length(), services[i].service) == 0) {
							servicesBody += "S";
							if (services[i].ipv4 == 0) {
								// Fill in the IP address of the interface we are listening on.
								uint32_t ipv4;
								std::string ipv6;
								if (!remoteToLocalIP(sender, ipv4, ipv6)) {
									std::cerr << "Failed to convert remote IP to local." << std::endl;
									continue;
								}
								
								if (ipv6.length() > 39) {
									std::cerr << "Got wrong ipv6 string length: " << ipv6.length() 
												<< std::endl;
									continue;
								}
								
								//ipv4 = bb.toGlobal(ipv4, he);
								servicesBody += std::string((char*) &ipv4, 4);
								uint8_t ipv6len = ipv6.length();
								servicesBody += (char) ipv6len;
								servicesBody += ipv6;
							}
							else {
								//uint32_t ipv4 = bb.toGlobal(services[i].ipv4, he);
								uint32_t ipv4 = services[i].ipv4;
								servicesBody += std::string((char*) &ipv4, 4);
								uint8_t ipv6len = services[i].ipv6.length();
								servicesBody += (char) ipv6len;
								servicesBody += services[i].ipv6;
							}
							
							uint16_t hlen = services[i].hostname.length();
							//hlen = bb.toGlobal(hlen, he);
							servicesBody += std::string((char*) &hlen, 2);
							servicesBody += services[i].hostname;
							
							//uint16_t port = bb.toGlobal(services[i].port, he);
							uint16_t port = services[i].port;
							servicesBody += std::string((char*) &port, 2);
							servicesBody += (char) (services[i].protocol);
							
							uint16_t snlen = services[i].service.length();
							//snlen = bb.toGlobal(snlen, he);
							servicesBody += std::string((char*) &snlen, 2);
							servicesBody += services[i].service;
							
							scount++;
						}
					}
					servicesMutex.unlock();
				}
				else {
					// Create response body containing all local data.
					servicesMutex.lock();
					for (int i = 0; i < services.size(); ++i) {
						servicesBody += "S";
						if (services[i].ipv4 == 0) {
							// Fill in the IP address of the interface we are listening on.
							uint32_t ipv4;
							std::string ipv6;
							if (!remoteToLocalIP(sender, ipv4, ipv6)) {
								std::cerr << "Failed to convert remote IP to local." << std::endl;
								continue;
							}
							
							if (ipv6.length() > 39) {
								std::cerr << "Got wrong ipv6 string length: " << ipv6.length() 
											<< std::endl;
								continue;
							}
							
							//ipv4 = bb.toGlobal(ipv4, he);
							servicesBody += std::string((char*) &ipv4, 4);
							uint8_t ipv6len = ipv6.length();
							servicesBody += (char) ipv6len;
							servicesBody += ipv6;
						}
						else {
							//uint32_t ipv4 = bb.toGlobal(services[i].ipv4, he);
							uint32_t ipv4 = services[i].ipv4;
							servicesBody += std::string((char*) &ipv4, 4);
							uint8_t ipv6len = services[i].ipv6.length();
							servicesBody += (char) ipv6len;
							servicesBody += services[i].ipv6;
						}
						
						uint16_t hlen = services[i].hostname.length();
						//hlen = bb.toGlobal(hlen, he);
						servicesBody += std::string((char*) &hlen, 2);
						servicesBody += services[i].hostname;
							
						//uint16_t port = bb.toGlobal(services[i].port, he);
						uint16_t port = services[i].port;
						servicesBody += std::string((char*) &port, 2);
						servicesBody += (char) (services[i].protocol);
						
						uint16_t snlen = services[i].service.length();
						//snlen = bb.toGlobal(snlen, he);
						servicesBody += std::string((char*) &snlen, 2);
						servicesBody += services[i].service;
						
						scount++;
					}
					
					servicesMutex.unlock();
				}
				
#ifdef DEBUG
				std::cout <<"Services body generated of size: " << servicesBody.length() << std::endl;
#endif
				
				// Assemble the full response message.
				std::string msg = "NYANSD";
				uint16_t msglen = 1;
				uint8_t type = (uint8_t) NYSD_MESSAGE_TYPE_RESPONSE;

				msglen += servicesBody.length() + 1;	// Add 1 for service section counter.
				//msglen = bb.toGlobal(msglen, he);
				msg += std::string((char*) &msglen, 2);
				msg += (char) type;
				msg += std::string((char*) &scount, 1);
				msg += servicesBody;
				
#ifdef DEBUG
				std::cout << "Sending response with size: " << msg.length() << std::endl;
#endif
				
				int n = udpsocket.sendTo(msg.data(), msg.length(), sender);
			}
		}
	}
	
	// Clean up resources and return.
	return;
}
