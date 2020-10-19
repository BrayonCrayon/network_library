// ClientApp.cpp : Defines the entry point for the console application.
//
// UDP Client
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <SocketWrapper.hpp>
#include <ThreadPool.hpp>

using namespace std;

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
//#include "ClientSocket.h"
#pragma comment (lib,"ws2_32.lib")
/*
* UDP
* - is good for streaming videos
* - UDP doesnt report missing packets(this is because it doesnt know what is coming)
* - very fast
* - reliability is on you
* - good for serving multiple people since its just listening
*
* TCP
* - is used for things like FTP.
* - TCP will let you know when packets are missing ( a connection is established and knows that packets are coming)
* - requires a dedicated connection to every client
*/


int const						MAXLINE = 256;
unsigned short constexpr		PORT = 49153;


class ChatClient
{
	bool quit = false;
	bool isThreadListening = false;
	Socket_Wrapper preMortem;
public:
	bool comChannel = false;
	string userName = "";

	ChatClient(string uName)
	{
		this->userName = uName;
		preMortem.SocketSetup("127.0.0.1", PORT, false); // Let the games begin! 192.168.1.145
	}

	~ChatClient() {}

	VOID ReceiveMessage(ChatClient& c_ref)
	{
		string line = "";
		while(true)
		{

			{
				unique_lock<mutex> ul(recPool.wake_lock);
				recPool.wake_cond.wait(ul);
				c_ref.isThreadListening = true;
			}
			if (c_ref.isThreadListening && !this->quit)
			{
				c_ref.isThreadListening = false;
				preMortem.UDPReceive(line);
			}

			if (!this->quit && line != "")
			{
				cout << ">" << line << endl;
			}

			{
				lock_guard<mutex> wL(recPool.work_lock);
				if (this->quit)
				{
					recPool.wake_cond.notify_all();
					break;
				}
				else
					recPool.wake_cond.notify_one();
			}
		}
	}


	Thread_Pool<ChatClient, void, ChatClient&> recPool;
	VOID StartChatClient()
	{

		string line = userName + ":entered";
		preMortem.UDPClientSend(line);
		cout << line << endl;
		// start receiving thread pool to listen for incoming messages
		recPool.SetupPool(&ChatClient::ReceiveMessage, *this, *this);
		recPool.wake_cond.notify_one();
	}



	void ParticipateInChat()
	{
		int const MAXLINE = 256;
		string line("");

		for (;;)
		{
			if (getline(cin, line))
			{
				line = userName + ":" + line;

				if (!line.empty() && line.substr(line.find_first_of(':') + 1, line.length()) == "quit")
				{
					preMortem.UDPClientSend(line);
					break;
				}
				
				preMortem.UDPClientSend(line);
			}
		}
		this->quit = true;
		recPool.~Thread_Pool();
	}

};


int main(int argc, char* argv[]) {
	for (int i = 0; i < argc; ++i)
	{
		cout << argv[i] << endl;
	}

	if (argc < 2)
	{
		cout << "Error: No argument provided for Client name." << endl;
		return EXIT_FAILURE;
	}

	ChatClient myClient(argv[1]);

	myClient.StartChatClient();
	myClient.ParticipateInChat();
}
