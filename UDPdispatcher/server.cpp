//=================================================================================================
// server.cpp		deal with the central host
//=================================================================================================

#include "framework.h"

_server::_server(unsigned short p)
{
	port = p;
	if(p) sthread = new std::thread([this](){ worker(); });
}
_server::~_server()
{
	stop = true;
	sthread->join();		// wait for worker to end
}
void _server::worker()
{
	// create socket
	SOCKET server_socket;
	while((server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET){ // <<< UDP socket
		if(error("S:", WSAGetLastError()))
			return;
	}

	// setup address structure
	sockaddr_in server_addr{};
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr.s_addr);	// any address
	server_addr.sin_port = htons(port);								// fixed port

	// bind
	if(bind(server_socket, (sockaddr*)&server_addr, sizeof server_addr) == SOCKET_ERROR)
		stop = error("S:", WSAGetLastError());

	// communication loop
	sockaddr_in client_addr{};			// save the client id we are receiving from so we can transmit back to it
	while(!stop){
		// first do the read
		if(WillNotBlock(server_socket, SKT_READ)){
			// read the data
			char message[512] = {};
			int slen = sizeof sockaddr_in;
			int message_length{};
			if((message_length = recvfrom(server_socket, message, sizeof message, 0, (sockaddr*)&client_addr, &slen)) != SOCKET_ERROR){
				blob *b = new blob(message, message_length);
				received.enqueue(*b);
				++nRecv;
				continue;					// if we receive try again immediately
			}
			// it didn't read
			stop = error("S:", WSAGetLastError());
		}

		// is there anything to send (and is there anybody to send too yet)?
		if(client_addr.sin_family!=0 && !toSend.empty() && WillNotBlock(server_socket, SKT_WRITE)){
			blob b = toSend.dequeue();
			// send the message
			if(sendto(server_socket, (const char*)b.data, b.length, 0, (sockaddr*)&client_addr, sizeof sockaddr_in) == SOCKET_ERROR)
				stop = error("S:", WSAGetLastError());
			else
				++nSent;
			continue;			// straight back to stop check
		}

		Sleep(1);		// relinquish our time slot
	}
	closesocket(server_socket);
}
