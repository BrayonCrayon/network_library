#include <SocketWrapper.hpp>
#define _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;

/*	Method Definition
	Method:		~Socket_Wrapper
	Params:		Void
	Returns:	Void
	Purpose:	To call the clean up socket methods
*/
Socket_Wrapper::~Socket_Wrapper()
{
	socketAdapter.SocketCleanUp(hSocket);
	socketAdapter.WSACleanUp();
}

/*	Method Definition
	Method:		SocketSetup
	Params:		string const&, unsigned short, IPPROTO 
	Returns:	VOID
	Purpose:	To setup the socket with the provided IP Address, Port number, and type of
					Protocal to use for either client, or server use.
*/
VOID Socket_Wrapper::SocketSetup(string const& ip, unsigned short port, bool isTCP)
{
	int result = socketAdapter.SocketStartup(this->wsaData);

	if (result != WSA_STARTUP_ERROR)
		return;

	this->isTcp_ = isTCP;
	this->serverAddress.sin_family = AF_INET;
	this->serverAddress.sin_port = htons(port);

	if (this->isTcp_)
	{
		this->hSocket = socketAdapter.TCPSocket();
		this->serverAddress.sin_addr.s_addr = inet_addr(ip.c_str());
	}
	else
	{
		this->hSocket = socketAdapter.UDPSocket();
		if (ip.empty())
			this->serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		else
			this->serverAddress.sin_addr.s_addr = inet_addr(ip.c_str());
	}
}

/*	Method Definition
	Method:		ServerSetup
	Params:		Void
	Returns:	int
	Purpose:	To setup the server with the provided Protocal given in SocketSetup()
				NOTE*** Check for Failure when using this method.
				NOTE*** The method will return EXIT_FAILURE if the 'ServerSetup()' is
						called before 'SocketSetup()'
*/
int Socket_Wrapper::ServerSetup()
{
	int bindResult = SOCKET_ERROR;
	int tcpListenResult = SOCKET_ERROR;

	switch (this->isTcp_)
	{
	case 1:
		bindResult = socketAdapter.TCPBind(hSocket, this->serverAddress);
		tcpListenResult = socketAdapter.TCPListen(hSocket);

		break;

	case 0:
		bindResult = socketAdapter.UDPBind(hSocket, this->serverAddress);

		break;
	}

	if (bindResult == SOCKET_ERROR && tcpListenResult == SOCKET_ERROR)
		return EXIT_FAILURE;

	return SERVER_SETUP_SUCCEED;
}


/*	Method Definition
	Method:		TCPConnect
	Params:		Void
	Returns:	int
	Purpose:	To connect to the server with the socket created and the serveraddresss
					that was made in 'SocketSetup()'
*/
int Socket_Wrapper::TCPConnect()
{
	return socketAdapter.Connect(this->hSocket, this->serverAddress);
}

/*	Method Definition
	Method:		TCPSend
	Params:		string&
	Returns:	int
	Purpose:	Send a msg from the client side to the server
					using TCP protocal.
				NOTE*** each message sent is split into messages 256 characters long
						and sent to the client or server
*/
int Socket_Wrapper::TCPSend(string& msg, SOCKET* client)
{
	char* sendBuffer = nullptr;
	for (size_t startPos = 0, endPos;;)
	{
		endPos = (RECEIVE_LENGTH < msg.length()) ? RECEIVE_LENGTH : msg.length();

		strcpy_s(sendBuffer, msg.size(), msg.substr(startPos, endPos).c_str()); 
		
		
		if (client == nullptr)
			socketAdapter.Send(hSocket, &sendBuffer);
		else
			socketAdapter.Send(*client, &sendBuffer);


		msg.erase( (msg.begin() + startPos), (msg.begin() + endPos) );

		if (msg.empty())
			break;
	}

	return SERVER_SETUP_SUCCEED;
}

/*	Method Definition
	Method:		TCPReceive
	Params:		string&
	Returns:	VOID
	Purpose:	Recieves a message in text form from 
					either a client or server using TCP protocal
				NOTE*** each message recieved is only allocated for 256 
							characters long.
*/
VOID Socket_Wrapper::TCPReceive(string& recMsg, SOCKET* client)
{	
	char* recvBuffer[RECEIVE_LENGTH];
	
	if (client == nullptr)
		socketAdapter.Recieve(hSocket, recvBuffer);
	else
		socketAdapter.Recieve(*client, recvBuffer);

	recMsg = *recvBuffer;
}


/*	Method Definition
	Method:		UDPSend
	Params:		string&
	Returns:	int
	Purpose:	Send a msg from the client side to the server
					using UDP protocal.
				NOTE*** each message sent is split into messages 256 characters long
						and sent to the client or server
*/
int Socket_Wrapper::UDPSend(string& msg)
{

	socklen_t sockLen = sizeof(this->clientAddress);

	for (size_t startPos = 0, endPos;;)
	{
		endPos = (RECEIVE_LENGTH < msg.length()) ? RECEIVE_LENGTH : msg.length();

		socketAdapter.UDPSendTo(hSocket, this->clientAddress, sockLen, msg.substr(startPos, endPos));
		msg.erase((msg.begin() + startPos), (msg.begin() + endPos));

		if (msg.empty())
			break;
	}
	
	return SERVER_SETUP_SUCCEED;
}

/*	Method Definition
	Method:		UDPClientSend
	Params:		string&
	Returns:	int
	Purpose:	Send a msg from the client side to the server
				using UDP protocal.
				NOTE*** each message sent is split into messages 256 characters long
				and sent to the client or server
*/
int Socket_Wrapper::UDPClientSend(string& msg)
{

	socklen_t sockLen = sizeof(this->serverAddress);

	for (size_t startPos = 0, endPos;;)
	{
		endPos = (RECEIVE_LENGTH < msg.length()) ? RECEIVE_LENGTH : msg.length();

		socketAdapter.UDPSendTo(hSocket,  *(SOCKADDR*)&this->serverAddress, sockLen, msg.substr(startPos, endPos));
		msg.erase((msg.begin() + startPos), (msg.begin() + endPos));

		if (msg.empty())
			break;
	}

	return SERVER_SETUP_SUCCEED;
}


/*	Method Definition
	Method:		UDPSpecificSend
	Params:		string&, SOCKADDR&
	Returns:	VOID
	Purpose:	Send a msg from the server side to a specific client using UDP protocal.
				
				NOTE*** each message sent is split into messages 256 characters long
					and sent to the client or server
*/
VOID Socket_Wrapper::UDPSpecificSend(string msg, SOCKADDR& clientAddress)
{
	socklen_t sockLen = sizeof(clientAddress);

	for (size_t startPos = 0, endPos;;)
	{
		endPos = (RECEIVE_LENGTH < msg.length()) ? RECEIVE_LENGTH : msg.length();

		socketAdapter.UDPSendTo(hSocket, clientAddress, sockLen, msg.substr(startPos, endPos));
		msg.erase((msg.begin() + startPos), (msg.begin() + endPos));

		if (msg.empty())
			break;
	}
}

/*	Method Definition
	Method:		UDPReceive
	Params:		string&
	Returns:	VOID
	Purpose:	Recieves a message in text form from
					either a client or server using UDP protocal
				NOTE*** each message recieved is only allocated for 256
							characters long.
*/
VOID Socket_Wrapper::UDPReceive(string& recMsg)
{
	socklen_t sockLen = sizeof(this->clientAddress);
	char recvBuffer[RECEIVE_LENGTH];
	int result = socketAdapter.UDPRecvFrom(hSocket, this->clientAddress, sockLen, recvBuffer, RECEIVE_LENGTH);

	if (result == FAILED_RETURN)
		return;
	
	recvBuffer[min(result, RECEIVE_LENGTH - 1)] = 0;
	recMsg = recvBuffer;
}


/*	Method Definition
	Method:		UDPReceive
	Params:		string&
	Returns:	VOID
	Purpose:	Recieves a message in text form from either a client or server using UDP protocal, and
						returns the client or server address that was sent.

				NOTE*** each message recieved is only allocated for 256 characters long.
*/
SOCKADDR Socket_Wrapper::UDPReceiveClient(string& recMsg)
{
	SOCKADDR newClient;
	socklen_t sockLen = sizeof(newClient);
	char recvBuffer[RECEIVE_LENGTH];
	int result = socketAdapter.UDPRecvFrom(hSocket, newClient, sockLen, recvBuffer, RECEIVE_LENGTH);
	
	if (result == FAILED_RETURN)
		return newClient;

	recvBuffer[min(result, RECEIVE_LENGTH - 1)] = 0;
	recMsg = recvBuffer;

	return newClient;
}

/*	Method Definition
	Method:		TCPAccept
	Params:		SOCKET&
	Returns:	int
	Purpose:	Trys to accept a client on the server with a provided socket
					using TCP protocal
				NOTE*** If the return value is a SOCKET_ERROR than it did 
							not accept a client, if it's a value of ACCEPT_SUCCEEDED
							then it accepted a client with the provided socket.
*/
int Socket_Wrapper::TCPAccept(SOCKET& hAcceptSocket)
{
	hAcceptSocket = socketAdapter.TCPAccept(this->hSocket);


	if (hAcceptSocket == SOCKET_ERROR)
		return SOCKET_ERROR;
	else
		return ACCEPT_SUCCEEDED;
}


/*	Method Definition
	Method:		CleanUpClient
	Params:		vector<SOCKET>&
	Returns:	VOID
	Purpose:	To close an array of clients that are
					associated with a SOCKET handle
*/
VOID Socket_Wrapper::CleanUpClient(vector<SOCKET>& clients)
{
	for (size_t idx = 0; idx < clients.size(); ++idx)
		socketAdapter.SocketCleanUp(clients[idx]);
}

/*	Method Definition
	Method:		CleanUpClient
	Params:		SOCKET&
	Returns:	VOID
	Purpose:	To close a client that is associated with a SOCKET handle
*/
VOID Socket_Wrapper::CleanUpClient(SOCKET& client)
{
	socketAdapter.SocketCleanUp(client);
}

