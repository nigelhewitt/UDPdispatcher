// client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cstdio>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib") 
#pragma warning(disable:4996) 

#define SERVER "127.0.0.1"  // or "localhost" - ip address of UDP server
#define BUFLEN 512  // max length of answer
#define PORT 2239  // the port on which to listen for incoming data

//=====================================================================================================
// handler to unpack Windows error codes into text  (Work in Wide so we can handle anything)
//=====================================================================================================

void error(DWORD err)
{
	char temp[200];
	int cb = sizeof temp;
	if(err == 0)
		err = GetLastError();
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), temp, sizeof temp, nullptr);
	printf("%s", temp);
}

//=================================================================================================
// initialise winsock
//=================================================================================================

bool init()
{
	WSADATA ws;
	printf("Initialising Winsock...\n");
	if(WSAStartup(MAKEWORD(2, 2), &ws) != 0){
		printf("Failed. Error Code: ");
		error(WSAGetLastError());
		return false;
	}
	return true;
}
//=================================================================================================
// send and receive messages to named server
//=================================================================================================

bool client(const char* url, u_short port)
{
	// create socket
	sockaddr_in server;
	SOCKET client_socket;
	if((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR){ // <<< UDP socket
		printf("socket() failed with error code: %d", WSAGetLastError());
		return false;
	}

	// setup address structure
	memset((char*)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.S_un.S_addr = inet_addr(url);

	// start communication
	while(true){
		char message[BUFLEN];
		printf("Enter message: ");
		gets_s(message, BUFLEN);

		// send the message
		if(sendto(client_socket, message, (int)strlen(message), 0, (sockaddr*)&server, sizeof sockaddr_in) == SOCKET_ERROR){
			printf("sendto() failed with error code: %d", WSAGetLastError());
			return false;
		}

		// receive a reply and print it
		// clear the answer by filling null, it might have previously received data
		char answer[BUFLEN] = {};

		// fix the socket so it doesn't block
		DWORD mSecs = 1000;
		setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&mSecs, sizeof mSecs);

		// try to receive some data, this is a blocking call
		int slen = sizeof sockaddr_in;
		int answer_length;
		DWORD err;
again:
		if((answer_length = recvfrom(client_socket, answer, BUFLEN, 0, (sockaddr*)&server, &slen)) == SOCKET_ERROR){
			err = WSAGetLastError();
			if(err==10060){
				Sleep(500);
				printf(".");
				goto again;
			}
			printf("recvfrom() failed with error code: %d, %x\n", err, err);
			error(err);
			return false;
		}

		printf("\n%s\n", answer);
	}
	closesocket(client_socket);
	return true;
}

int main()
{
	printf("UDP Client\n");
	if(!init())
		exit(1);
	printf("Initialised.\n");

	client(SERVER, PORT);
	WSACleanup();
}
