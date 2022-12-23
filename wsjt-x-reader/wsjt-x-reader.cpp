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

#define BUFLEN 512
#define PORT 8888

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

void dump(FILE* f, void* buffer, int cb)
{
	char temp[100];
	uint8_t *buf = (uint8_t*)buffer;
	for(int i=0; i<cb; i+=16){
		int n = cb-i, j;
		if(n>16) n=16;
		sprintf(temp, "\r\n%04X ", i);
		for(j=0; j<n; j++)
			sprintf(temp+7+j*3, "%02X ", buf[i+j]);
		for( ;j<16; j++)
			strcpy(temp+7+j*3, "   ");
		strcpy(temp+55, "   ");
		for(j=0; j<n; j++){
			uint8_t c= buf[i+j];
			if(c<0x20 || c>=0x7f) c = ' ';
			temp[58+j*2] = c;
			temp[59+j*2] = ' ';
		}
		temp[57+j*2] = '\n';
		temp[58+j*2] = 0;
		fwrite(temp, 1, 58+j*2, f);
	}
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

	// prepare the sockaddr_in structure
	sockaddr_in server;
	server.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &server.sin_addr.s_addr);			// new format
	server.sin_port = htons(PORT);

	// bind
	if(bind(server_socket, (sockaddr*)&server, sizeof server) == SOCKET_ERROR){
		printf("Bind failed with error code: %d", WSAGetLastError());
		return false;
	}
	printf("Bind done\n");

	// open the save file
	FILE* fh;
	if(fopen_s(&fh, "dump.edc", "w")!=0){
		printf("unable to open \"../../../dump.edc\"\r\n");	// in project folder
		return false;
	}
	fprintf(fh, "Log file opened\n");

	sockaddr_in client{};
	while(true){

		if(WillNotBlock(server_socket, SKT_READ)){

			// try to receive some data
			char message[BUFLEN]{};
			int message_len{};
			int slen = sizeof sockaddr_in;
			if(message_len = recvfrom(server_socket, message, BUFLEN, 0, (sockaddr*)&client, &slen) == SOCKET_ERROR){
				printf("recvfrom() failed with error code: %d", WSAGetLastError());
				return false;
			}

			// print details of the client/peer and the data received
			printf("Received packet from %s:%d of %d bytes\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), message_len);
			dump(fh, message, message_len);
//			printf("Data: %d\n", message_len);
		}
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
		Sleep(1);
	}
	closesocket(server_socket);
	fprintf(fh, "Log file closed\n");
	fclose(fh);
	return true;
}

int main()
{
	system("UDP Server");
	if(!init()) exit(1);

	printf("Initialised.\n");
	if(!serve()) exit(1);

	WSACleanup();
	exit(0);
}
