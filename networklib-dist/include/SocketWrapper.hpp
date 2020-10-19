#pragma once
#include <SocketAdapter.hpp>
#include <vector>

#if defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "networklib-mt-gd.lib")
#elif defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "networklib-mt-sgd.lib")
#elif !defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "networklib-mt.lib")
#elif !defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "networklib-mt-s.lib")
#endif

#ifndef SOCKET_WRAPPER_HPP
#define SOCKET_WRAPPER_HPP
#define _WINSOCK_DEPRECATED_NO_WARNINGS

constexpr short WSA_STARTUP_ERROR = 0;
constexpr short SERVER_SETUP_SUCCEED = 0;
constexpr short RECEIVE_LENGTH = 256;
constexpr short ACCEPT_SUCCEEDED = 1;
constexpr short FAILED_RETURN = -1;

class Socket_Wrapper
{
	// ThinAdapter
	Socket_Adapter socketAdapter;
	
	// Socket Definitions
	SOCKET hSocket;
	WSAData wsaData;
	sockaddr_in serverAddress = {0};

	sockaddr clientAddress;
	bool isTcp_;

public:
	Socket_Wrapper() {}
	~Socket_Wrapper();
	
	//Main methods for server/client use
	VOID	SocketSetup(std::string const&, unsigned short, bool);
	int		ServerSetup();

	//Main TCP communication
	int 	TCPSend(std::string&, SOCKET*);
	int 	TCPConnect();
	int		TCPAccept(SOCKET&);
	VOID	TCPReceive(std::string&, SOCKET*);
	VOID	CleanUpClient(std::vector<SOCKET>&);
	VOID	CleanUpClient(SOCKET&);

	//Main UDP Communication
	int		UDPClientSend(std::string&);
	int		UDPSend(std::string&);
	VOID	UDPReceive(std::string&);
	VOID	UDPSpecificSend(std::string, SOCKADDR&);
	SOCKADDR UDPReceiveClient(std::string&);
};

#endif // !SOCKET_WRAPPER_HPP
