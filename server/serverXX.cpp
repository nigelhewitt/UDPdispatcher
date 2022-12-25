// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cstdio>
#include <conio.h>
#include <winsock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib") // Winsock Library
#pragma warning(disable:4996) 

#define BUFLEN 512
#define PORTX 2239

bool init()
{
	// initialise winsock
	WSADATA wsa;
	printf("Initialising Winsock...");
	if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
		printf("Failed. Error Code: %d", WSAGetLastError());
		return false;
	}
	return true;
}

enum SKT_MODE { SKT_READ, SKT_WRITE };
bool WillNotBlock(SOCKET socket, SKT_MODE mode)
{
	WSAPOLLFD skt{};					// check if the socket can read/write without blocking
	skt.fd = socket;
	skt.events = mode==SKT_READ ? POLLRDNORM : POLLWRNORM;
	skt.revents = 0;
	return WSAPoll(&skt, 1, 0)>0;
}

bool serve(u_short port)		// aka 16 bit
{
	// create a socket
	SOCKET server_socket;
	if((server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET){
		printf("Could not create socket: %d", WSAGetLastError());
		return false;
	}
	printf("Socket created.\n");

	// prepare the sockaddr_in structure
	sockaddr_in server;
	memset(&server, 0, sizeof server);
	server.sin_port = htons(port);
	server.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr.s_addr);			// new format

	// bind
	if(bind(server_socket, (sockaddr*)&server, sizeof server) == SOCKET_ERROR){
		printf("Bind failed with error code: %d", WSAGetLastError());
		return false;
	}
	printf("Bind done\n");

	sockaddr_in client;
	memset(&client, 0, sizeof client);

	while(true){

		if(WillNotBlock(server_socket, SKT_READ)){
			fflush(stdout);
			char message[BUFLEN] = {};

			// try to receive some data
			int message_len;
			int slen = sizeof sockaddr_in;
			if(message_len = recvfrom(server_socket, message, BUFLEN, 0, (sockaddr*)&client, &slen) == SOCKET_ERROR){
				printf("recvfrom() failed with error code: %d", WSAGetLastError());
				return false;
			}

			// print details of the client/peer and the data received
			printf("Received packet from %s:%d of %d bytes\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), message_len);
//			printf("Data: %d\n", message_len);
		}
		if(_kbhit()){
			printf("\r\nEnter reply: ");
			char message[BUFLEN] = {};
			gets_s(message, BUFLEN);

			// reply to the client
			if(sendto(server_socket, message, (int)strlen(message), 0, (sockaddr*)&client, sizeof sockaddr_in) == SOCKET_ERROR){
				printf("sendto() failed with error code: %d", WSAGetLastError());
				return false;
			}
		}
		Sleep(1);
	}
	closesocket(server_socket);
	return true;
}

int main()
{
	system("UDP Server");
	if(!init()) exit(1);

	printf("Initialised.\n");
	if(!serve(PORTX)) exit(1);

	WSACleanup();
	exit(0);
}
