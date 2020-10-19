#include <SocketAdapter.hpp>
#define _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;

/*	Method Definition
	Method:		SocketStartup
	Params:		WSAData&
	Returns:	int
	Purpose:	Starts up the Windows Socket DLL implementation as a process
*/
 int Socket_Adapter::SocketStartup(WSAData& wsaData)
{
	return WSAStartup(MAKEWORD(2, 2), &wsaData);
}

 /*	Method Definition
	 Method:	TCPSocket
	 Params:	VOID
	 Returns:	VOID	
	 Purpose:	Sets the socket with the required internetwork, sock_stream, and protocal type
 */
 SOCKET Socket_Adapter::TCPSocket()
{
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

 /*	Method Definition
	 Method:	UCPSocket
	 Params:	VOID
	 Returns:	VOID
	 Purpose:	Sets the socket with the required internetwork, SOCK_DGRAM, and protocal type
 */
 SOCKET Socket_Adapter::UDPSocket()
 {
	 return socket(AF_INET, SOCK_DGRAM, 0);
 }

 /*	Method Definition
	 Method:	SocketCleanUp
	 Params:	SOCKET&
	 Returns:	VOID
	 Purpose:	Closes the provided socket
 */
 VOID Socket_Adapter::SocketCleanUp(SOCKET& hSocket)
{
	closesocket(hSocket);
}

 /*	Method Definition
	 Method:	WSACleanUp
	 Params:	VOID
	 Returns:	VOID
	 Purpose:	Closes and terminates the process running the WSA Dll.
 */
 VOID Socket_Adapter::WSACleanUp()
 {
	 WSACleanup();
 }


 /*	Method Definition
	 Method:	Connect
	 Params:	SOCKET&, SOCKADDR_IN 
	 Returns:	int
	 Purpose:	Connects a provided socket to socket Address
 */
 int Socket_Adapter::Connect(SOCKET& hSocket, SOCKADDR_IN sockAddr)
{
	return connect(hSocket, (SOCKADDR*)&sockAddr, sizeof(sockaddr));
}

 /*	Method Definition
	 Method:	Send
	 Params:	SOCKET&, char* []
	 Returns:	int
	 Purpose:	sends a set of bytes to the listening socket.
 */
 int Socket_Adapter::Send(SOCKET& hSocket, char* sendBuff[])
{
	return send(hSocket, *sendBuff, static_cast<int>(strlen(*sendBuff) + 1), 0);
}

 /*	Method Definition
	 Method:	Recieve
	 Params:	SOCKET&, char* []
	 Returns:	int
	 Purpose:	Sets char[] to the bytes that are coming in, with 
					the connected socket.
 */
 int Socket_Adapter::Recieve(SOCKET& hSocket, char* recBuff[])
{
	return recv(hSocket, *recBuff, static_cast<int>(strlen(*recBuff)), 0);
}

 /*	Method Definition
	 Method:	TCPAccept
	 Params:	SOCKET&
	 Returns:	VOID
	 Purpose:	A Server TCP method to accept client connections
 */
 SOCKET Socket_Adapter::TCPAccept(SOCKET& hSocket)
{
	return accept(hSocket, NULL, NULL);
}

 /*	Method Definition
	 Method:	TCPListen
	 Params:	SOCKET&
	 Returns:	VOID
	 Purpose:	A server method to listen on a Socket for client
					connections
 */
 int Socket_Adapter::TCPListen(SOCKET& hSocket)
{
	return listen(hSocket, 1);
}

 /*	Method Definition
	 Method:	TcpBind
	 Params:	SOCKET&, SOCKADDR_IN& 
	 Returns:	int
	 Purpose:	Binds the provided socket to the serverAddress for TCP protocals
 */
 int Socket_Adapter::TCPBind(SOCKET& hSocket, SOCKADDR_IN& serverAddr)
{
	return bind(hSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr));
}

 /*	Method Definition
	 Method:	UDPBind
	 Params:	SOCKET&, SOCKADDR_IN&
	 Returns:	int
	 Purpose:	Binds the provided socket to the serverAddress for UDP protocals
 */
 int Socket_Adapter::UDPBind(SOCKET& hSocket, SOCKADDR_IN& sockAddr)
{
	return bind(hSocket, reinterpret_cast<SOCKADDR*>(&sockAddr) , sizeof(SOCKADDR_IN));
}

 /*	Method Definition
	 Method:	UDPRecvFrom
	 Params:	SOCKET&, SOCKADDR_IN&, socklen_t&, char* [], int 
	 Returns:	int
	 Purpose:	recieve information from the socket using UDP 
					protocal in bytes with a provided byte length
 */
 int Socket_Adapter::UDPRecvFrom(SOCKET& hSocket, SOCKADDR& serverAddr, socklen_t& sockLen, char msg[], int len)
{
	return recvfrom(hSocket, msg, len, 0, &serverAddr, &sockLen);
}

 /*	Method Definition
	 Method:	UDPSendTo
	 Params:	SOCKET&, SOCKADDR_IN&, socklen_t&, string const&
	 Returns:	int
	 Purpose:	Send a message in bytes to the reciever using a socket on a UDP protocal.
 */
 int Socket_Adapter::UDPSendTo(SOCKET& hSocket, SOCKADDR& sockAddr, socklen_t& sockLen, string const& msg)
{
	return sendto(hSocket, msg.c_str(), static_cast<int>(msg.size()), 0, &sockAddr, sockLen);
}