// wsjt-x-reader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdint.h>
#include <cstdio>
#include <conio.h>
#include <winsock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib") // Winsock Library
#pragma warning(disable:4996) 

// in unpack.cpp
bool unpack(FILE* fh, byte* &p, int &cb);
void dump(FILE* f, void* buffer, int cb);

#define PORT 2239

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

bool serve()
{
	// create a socket
	SOCKET server_socket;
	if((server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET){
		printf("Could not create socket: %d", WSAGetLastError());
		return false;
	}
	printf("Socket created.\n");

#if false
	DWORD bReuse{true};
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&bReuse, sizeof bReuse);
#endif

	// prepare the sockaddr_in structure
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr.s_addr);			// new format
	server_addr.sin_port = htons(PORT);

	// bind
	if(bind(server_socket, (sockaddr*)&server_addr, sizeof server_addr) == SOCKET_ERROR){
		printf("Bind failed with error code: %d", WSAGetLastError());
		return false;
	}
	printf("Bind OK\n");

	// open the save file
	FILE* fh;
	if(fopen_s(&fh, "../dump.edc", "wb")!=0){
		printf("unable to open \"../dump.edc\"\r\n");	// in project folder
		return false;
	}
	fprintf(fh, "Log file opened\n\n");

	sockaddr_in client{};

	while(true){
		if(WillNotBlock(server_socket, SKT_READ)){
			// try to receive some data
			char message[512]{};
			int slen = sizeof sockaddr_in;
			int message_len{};
			if((message_len = recvfrom(server_socket, message, sizeof message, 0, (sockaddr*)&client, &slen)) == SOCKET_ERROR){
				printf("recvfrom() failed with error code: %d", WSAGetLastError());
				return false;
			}

			// print details of the client/peer and the data received
			printf("Received packet from %s:%d of %d bytes\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), message_len);
			int cb = message_len;
			byte* bx = (byte*)message;
			if(!unpack(fh, bx, cb))
				dump(fh, message, message_len);
//			printf("Data: %s\n", message);
		}
#if false
		char message[512]{};
		printf("Enter reply: ");
		gets_s(message, sizeof message);

		// reply to the client
		if(sendto(server_socket, message, (int)strlen(message), 0, (sockaddr*)&client, sizeof sockaddr_in) == SOCKET_ERROR){
			printf("sendto() failed with error code: %d", WSAGetLastError());
			return false;
		}
#endif
#if true
		if(_kbhit())
			break;
/*			printf("\r\nEnter reply: ");
			char message[BUFLEN] = {};
			gets_s(message, BUFLEN);

			// reply to the client
			if(sendto(server_socket, message, (int)strlen(message), 0, (sockaddr*)&client, sizeof sockaddr_in) == SOCKET_ERROR){
				printf("sendto() failed with error code: %d", WSAGetLastError());
				return false;
			}
		}
*/
#endif

		Sleep(1);
	}
	closesocket(server_socket);
	fprintf(fh, "Log file closed\n");
	fclose(fh);
	return true;
}

int main()
{
	printf("UDP reader");
	if(!init()) exit(1);

	printf("Initialised.\n");
	if(!serve()) exit(1);

	WSACleanup();
	exit(0);
}
