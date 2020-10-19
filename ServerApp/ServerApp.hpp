#pragma once
#include <SocketWrapper.hpp>
#include <ThreadPool.hpp>
#include "ServerConstants.hpp"
#include <string>
#include <map>
#include <locale>

#ifndef SERVER_APP_HPP
#define SERVER_APP_HPP

// Server Application to host the server.
class ServerApp
{
	// Server Deffinitions
	Socket_Wrapper sockWrap_;
	std::string ipName_;
	unsigned short port_;
	bool isShuttingDown = false;

	// Connected Clients
	std::map<std::string, sockaddr*> clients_;
	std::map<std::string, sockaddr*>::iterator client_Iter;
	bool isClientsGone = false;

	// Server Setup Failed
	bool isSetupDone = false;

	// Task arguments
	typedef std::tuple<std::string&> task_type;

	// Thread Functions
	void TransmitMsg(std::string, ServerApp&);
	void ReciveMsg(std::string, ServerApp&);

	// Server Helper methods
	std::string toLowerString(std::string const&);
	void removeClient(std::string const& clientName);
	void addClient(std::string const& clientName, SOCKADDR&);

public:

	ServerApp(std::string ip = std::string(""), unsigned short port = 49153) : ipName_(ip), port_(port)
	{
		// Setup server with ipaddres, and port number
		sockWrap_.SocketSetup(ipName_, port_, false);
		
		if (sockWrap_.ServerSetup() == SERVER_SETUP_SUCCEED)
			isSetupDone = true;

	}
	~ServerApp() {}

	void StartServer(ServerApp&);

private:
	Thread_Pool<ServerApp, void, std::string, ServerApp&> recieve_pool_;
	Thread_Pool<ServerApp, void, std::string, ServerApp&> transmit_pool_;
};

/*	Method Definition
	Method:		removeClient
	Params:		string const&
	Returns:	Void
	Purpose:	To remove the provided client name from the list of recorded clients.
					NOIE** if there are no more recoreded clients talking to eachother then
								the 'isClientsGone' is set to true to end and stop the recieveing 
								worker threads for listening.
					NOTE*** If there is already more than one client in the list with the same
								provided name, then decrement the count.
*/
inline void ServerApp::removeClient(std::string const& clientName)
{
	client_Iter = clients_.find(clientName);
	if (client_Iter != clients_.end())
	{
		sockWrap_.UDPSpecificSend(clientName + ":quit", *client_Iter->second);
		clients_.erase(client_Iter);
	}

	if (clients_.empty())
		this->isClientsGone = true;
}

/*	Method Definition
	Method:		addClient
	Params:		string const&
	Returns:	Void
	Purpose:	Add a client to recored client list
				NOTE*** If there is a client name already taken then add 
						an increment count to the same name
*/
inline void ServerApp::addClient(std::string const& clientName, SOCKADDR& clientAddr)
{
	client_Iter = clients_.find(clientName);
	if (client_Iter != clients_.end())
		return;

	clients_[clientName] = &clientAddr;
}


#endif // !SERVER_APP_HPP
