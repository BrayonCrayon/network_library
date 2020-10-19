#pragma once

#ifndef SOCKET_ADAPTER_HPP
#define SOCKET_ADAPTER_HPP
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <wS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

#if defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "networklib-mt-gd.lib")
#elif defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "networklib-mt-sgd.lib")
#elif !defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "networklib-mt.lib")
#elif !defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "networklib-mt-s.lib")
#endif

class Socket_Adapter
{

public:
	Socket_Adapter() {}
	~Socket_Adapter() {}

	// TCP Methods
	int			SocketStartup(WSAData&);
	SOCKET			TCPAccept(SOCKET&);
	int				TCPListen(SOCKET&);
	int				TCPBind(SOCKET&, SOCKADDR_IN&);
	SOCKET			TCPSocket();

	// UDP Methods
	int				UDPBind(SOCKET&, SOCKADDR_IN&);
	SOCKET			UDPSocket();
	int				UDPRecvFrom(SOCKET&, SOCKADDR&, socklen_t&, char[], int);
	int				UDPSendTo(SOCKET&, SOCKADDR&, socklen_t&, std::string const&);

	// Socket Methods
	VOID			SocketCleanUp(SOCKET&);
	VOID			WSACleanUp();
	int				Connect(SOCKET&, SOCKADDR_IN);
	int				Send(SOCKET&, char*[]);
	int				Recieve(SOCKET&, char*[]);

};

#endif // !SOCKET_ADAPTER_HPP
