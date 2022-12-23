//=================================================================================================
// client.cpp		deal with the central host
//=================================================================================================

#include "framework.h"

_client::_client(int n, const char* _url, unsigned short p)
{
	id = n;
	port = p;
	url = _strdup(_url);
	cthread = new std::thread([this](){ worker(); });
}
_client::~_client()
{
	stop = true;
	cthread->join();
	delete url;
}

void _client::worker()
{
	char szID[10];
	sprintf_s(szID, sizeof szID, "C%d:", id);

again:
	// create socket
	SOCKET client_socket;
	while((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR){		// UDP socket
		if(error(szID, WSAGetLastError()))			// it's either stop and reconfigure or retry
			return;
	}

	// setup the address structure
	sockaddr_in server;
	memset((char*)&server, 0, sizeof server);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	inet_pton(AF_INET, url, &server.sin_addr.S_un.S_addr);

	// communication loop
	while(!stop){
		// is there anything to send?
		if(!toSend.empty() && WillNotBlock(client_socket, SKT_WRITE)){
			blob b = toSend.dequeue();
			// send the message
			if(sendto(client_socket, (char*)b.data, b.length, 0, (sockaddr*)&server, sizeof sockaddr_in) == SOCKET_ERROR)
				stop = error(szID, WSAGetLastError());
			else{
				++nSent;
				bActive = true;
			}
			continue;			// straight back to stop check
		}

		// try to receive a reply
		if(WillNotBlock(client_socket, SKT_READ)){
			// read the data
			char data[512] = {};
			int slen = sizeof sockaddr_in;
			int data_length;
			if((data_length = recvfrom(client_socket, data, sizeof data, 0, (sockaddr*)&server, &slen)) != SOCKET_ERROR){
				blob *b = new blob(data, data_length);
				received.enqueue(*b);
				++nRecv;
				bActive = true;
				continue;
			}
			// if we get here we didn't read
			DWORD err = WSAGetLastError();
			if(err==WSAECONNRESET){				// they went away and the socket closed/never opened
				closesocket(client_socket);
				bActive = false;
				Sleep(5000);					// not to fast or we break things
				goto again;						// retry from the top...
			}
			stop = error(szID, err);
		}
		Sleep(1);		// relinquish our time slot
	}
	bActive = false;
	closesocket(client_socket);
}
