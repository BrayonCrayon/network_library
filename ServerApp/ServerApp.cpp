/*
	Program Name:	ServerApp
	Programmer:		Brayon Alan Deroy
	Created:		1/1/2018
	it's Purpose:	To host a server using UDP protocal from the network Library.
					This application manages entered clients, and clients that leave the chat.

*/
#include "ServerApp.hpp"
#include <iostream>

#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h> 

#include <WinSock2.h>
#include <wS2tcpip.h>

//tells the compiler if you succeed then link this to the compiler
#pragma comment (lib, "ws2_32.lib")

using namespace std;

/*	Method Definition
	Method:		toLowerString
	Params:		string const&
	Returns:	Void
	Purpose:	To put all characters of the provided string 'word' to 
					lower case and return it for matching purposes.
*/
string ServerApp::toLowerString(string const& word)
{
	string lowerCase;
	locale loc;
	for (size_t idx = 0; idx < word.length(); ++idx)
		lowerCase.push_back(tolower(word[idx], loc));

	return lowerCase;
}


/*	Method Definition
	Method:		ReciveMsg
	Params:		string, ServerApp&
	Returns:	Void
	Purpose:	Thread function that waits for recieved message, and adds that message to the 
					transmit task list for sending out.
					Note*** If a new client enters the chat, that client will have a message output of (ex) "Foobar: entered" then add them to the client list
					NOTE*** If a client wants to quit their session, that client will send a message output of (ex):"Foobar: quit" then remove them from the client list
*/
void ServerApp::ReciveMsg(string recMsg, ServerApp& c_ref)
{
	bool isWork = true;
	SOCKADDR* clientAddr;
	string msg;
	while (isWork)
	{

		{
			unique_lock<mutex> wL(recieve_pool_.wake_lock);

			recieve_pool_.wake_cond.wait(wL);
			//check to see if any messages were sent
			if(!c_ref.isClientsGone)
				clientAddr = &sockWrap_.UDPReceiveClient(recMsg);
		}


		size_t dilimiterPos = recMsg.find_first_of(CLIENT_DILIMITER);
		//if a message was recieved then check to see if a client is being added
		//		or if a client is being removed. Then send the required message to all
		//		other clients.
		if (!recMsg.empty() && dilimiterPos != recMsg.npos)
		{
			lock_guard<mutex> tL(recieve_pool_.task_lock);
			msg = recMsg.substr(dilimiterPos + CLIENT_ADD_DILIMITER, recMsg.length());
			cout << "Message Recieved from " << recMsg.substr(CLIENT_START, recMsg.find_first_of(CLIENT_DILIMITER)) << ": " << msg << endl;

			if (msg == CLIENT_QUIT)
			{
				cout << "Client removed: " << recMsg.substr(CLIENT_START, recMsg.find_first_of(CLIENT_DILIMITER)) << endl;
				removeClient(recMsg.substr(CLIENT_START, recMsg.find_first_of(CLIENT_DILIMITER)));
			}
			else if (msg == CLIENT_ENTER)
			{
				cout << "Client added: " << recMsg.substr(CLIENT_START, recMsg.find_first_of(CLIENT_DILIMITER)) << endl;
				addClient(recMsg.substr(CLIENT_START, recMsg.find_first_of(CLIENT_DILIMITER)), *clientAddr);
			}
			transmit_pool_.add_task(&ServerApp::TransmitMsg, c_ref, recMsg, c_ref);
			transmit_pool_.wake_cond.notify_one();
			recMsg.clear();
		}

		{
			lock_guard<mutex> wL(recieve_pool_.work_lock);
			if (this->isClientsGone)
			{
				isWork = false;
				recieve_pool_.wake_cond.notify_all();
			}
			else
				recieve_pool_.wake_cond.notify_one();
		}

	}//end of big while loop
}

/*	Method Definition
	Method:		TransmitMsg
	Params:		string, ServerApp& 
	Returns:	Void
	Purpose:	Thread function to send messages to all clients listening on for a callback. It takes a message off 
				the task list and sends it out for clients to get.
				NOTE*** Every msg sent will contain the person of which it came from ex "FooBar: this is there msg".
*/
void ServerApp::TransmitMsg(string sendMsg, ServerApp& c_ref)
{
	bool haveTask = false;
	while (!c_ref.isShuttingDown)
	{
		{
			unique_lock<mutex> wL(c_ref.transmit_pool_.wake_lock);
			c_ref.transmit_pool_.wake_cond.wait(wL);
		}

		haveTask = false;
		{
			lock_guard<mutex> tL(c_ref.transmit_pool_.task_lock);
			if (!c_ref.transmit_pool_.queued_Tasks.empty())
			{
				sendMsg = get<0>(c_ref.transmit_pool_.queued_Tasks.front().GetArguments());
				haveTask = true;
				c_ref.transmit_pool_.queued_Tasks.pop();
			}
		}

		if (haveTask)
		{
			lock_guard<mutex> tL(c_ref.transmit_pool_.work_lock);
			// Send the required message to all clients except for the one that sent the message.
			for(map<string, SOCKADDR*>::iterator iter = c_ref.clients_.begin(); iter != c_ref.clients_.end(); ++iter)
			{
				if (sendMsg.substr(0, sendMsg.find_first_of(CLIENT_DILIMITER)) != iter->first)
				{
					cout << "Sending Message to " << iter->first  << ": " << sendMsg.substr(sendMsg.find_first_of(CLIENT_DILIMITER) + CLIENT_ADD_DILIMITER, sendMsg.length()) << endl;
					c_ref.sockWrap_.UDPSpecificSend(sendMsg, *iter->second);
				}
			}
			sendMsg.clear();
		}
	}
}

/*	Method Definition
	Method:		StartServer
	Params:		Void
	Returns:	Void
	Purpose:	To setup the thread pools to deal the sending and recieveing messaging.
*/
void ServerApp::StartServer(ServerApp& c_ref)
{
	cout << "Starting Server Log\n" << endl;
	recieve_pool_.SetupPool(&ServerApp::ReciveMsg, c_ref, string(), c_ref);
	transmit_pool_.SetupPool(&ServerApp::TransmitMsg, c_ref, string(), c_ref);
	recieve_pool_.wake_cond.notify_one();
	recieve_pool_.~Thread_Pool();
	this->isShuttingDown = true;
	transmit_pool_.wake_cond.notify_all();
	transmit_pool_.~Thread_Pool();
	cout << "\nEnding Server Log" << endl;
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	unique_ptr<ServerApp> app(new ServerApp());

	try {
		app->StartServer(*app);
	}
	catch (exception ex)
	{
		cout << "An error Ocurred: Please Restart Server" << endl;
	}

	app.reset();
	_CrtDumpMemoryLeaks();
}