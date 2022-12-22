//=================================================================================================
// client.cpp		deal with the central host
//=================================================================================================

#include "framework.h"

_client::_client(const char* _url, unsigned short p)
{
	port = p;
	url = _strdup(_url);
	cthread = new std::thread([this](){ worker(); });
}
_client::~_client()
{
	stop = true;
	cthread->join();
}

void _client::worker()
{
	// create socket
	sockaddr_in server;
	SOCKET client_socket;
	if((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR){ // <<< UDP socket
		stop = error();
	}

	// setup address structure
	memset((char*)&server, 0, sizeof server);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	inet_pton(AF_INET, url, &server.sin_addr.S_un.S_addr);

	// communication loop
	bActive = true;
	while(!stop){
		// is there anything to send?
		if(!toSend.empty()){
			blob b = toSend.dequeue();
			// send the message
			if(sendto(client_socket, (char*)b.data, b.length, 0, (sockaddr*)&server, sizeof sockaddr_in) == SOCKET_ERROR){
				stop = error();
			}
			else
				++nSent;
			continue;			// straight back to stop check
		}
		// try to receive a reply
		char data[512] = {};

		// fix the socket so it doesn't block
		if(false){						// ?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
			DWORD mSecs = 1000;
			setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&mSecs, sizeof mSecs);

			// try to receive some data, this is normally a blocking call
			int slen = sizeof sockaddr_in;
			int data_length;
			if((data_length = recvfrom(client_socket, data, sizeof data, 0, (sockaddr*)&server, &slen)) != SOCKET_ERROR){
				blob *b = new blob(data, data_length);		// smart (aka deep) copy
				received.enqueue(*b);			// smart copy
				++nRecv;
				delete b;
				continue;
			}
			// if we get here we didn't read, was it just a timeout?
			DWORD err = WSAGetLastError();
			if(err!=10060){				// ie. no data waiting
				stop = error(err);
			}
		}
		Sleep(1);		// relinquish our time slot
	}
	bActive = false;
	closesocket(client_socket);
}
