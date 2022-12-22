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
	if((server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET){ // <<< UDP socket
		stop = error();
	}

	// setup address structure
	sockaddr_in server_addr{};
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr.s_addr);	// any address
	server_addr.sin_port = htons(port);								// fixed port

	// bind
	if(bind(server_socket, (sockaddr*)&server_addr, sizeof server_addr) == SOCKET_ERROR){
		stop = error();
	}
	// communication loop
	sockaddr_in client_addr{};			// save the client id we are receiving from so we can transmit back to it
	while(!stop){
		// fix the socket so it doesn't block
		DWORD mSecs = 1000;
		setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&mSecs, sizeof mSecs);

		// try to receive some data, this is normally a blocking call
		char message[512] = {};
		int slen = sizeof sockaddr_in;
		int message_length{};
		if((message_length = recvfrom(server_socket, message, sizeof message, 0, (sockaddr*)&client_addr, &slen)) != SOCKET_ERROR){
			blob *b = new blob(message, message_length);
			received.enqueue(*b);
			++nRecv;
			continue;					// if we receive try again immediately
		}
		// it didn't read, was it just a normal timeout
		DWORD err = WSAGetLastError();
		if(err!=10060){				// ie. no data waiting
			stop = error(err);
		}
		// is there anything to send (and is there anybody to send too yet)?
		if(client_addr.sin_family!=0 && !toSend.empty()){
			blob b = toSend.dequeue();		// initialisation so not a copy
			const char* p;
			int n;
			b.pack(p, n);
			// send the message
			if(sendto(server_socket, p, n, 0, (sockaddr*)&client_addr, sizeof sockaddr_in) == SOCKET_ERROR){
				stop = error();
			}
			else
				++nSent;
			continue;			// straight back to stop check
		}

		Sleep(1);		// relinquish our time slot
	}
	closesocket(server_socket);
}
