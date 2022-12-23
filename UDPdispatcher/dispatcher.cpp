//=================================================================================================
// dispatcher.cpp		deals with moving data about
//=================================================================================================

#include "framework.h"

_dispatcher::_dispatcher()
{
	dthread = new std::thread([this](){ worker(); });
}
_dispatcher::~_dispatcher()
{
	stop = true;
	while(!dthread->joinable())
		Sleep(1);
	delete server;
	while(!clients.empty()){
		delete clients.back();		// delete closes the client down
		clients.pop_back();
	}
	dthread->join();
}
void _dispatcher::getCounts(int& sSent, int& sRecv, int& cSent, int& cRecv, int& nActive)
{
	server->getCounts(sSent, sRecv);
	int s=0, r=0;
	cSent = 0;
	cRecv = 0;
	nActive = 0;
	for(_client* &c : clients){
		c->getCounts(s, r);
		cSent += s;
		cRecv += r;
		if(c->active())
			++nActive;
	}
}

void _dispatcher::worker()
{
	// make new server
	server = new _server(config.server.port);

	// make the clients
	int id = 1;
	for(_config::client_details cd : config.clients){
		_client *c = new _client(id++, cd.url, cd.port);
		clients.push_back(c);
	}

	while(!stop){
		if(!server->received.empty()){
			blob b = server->received.dequeue();
			// send blob to the clients
			for(_client* &c : clients)
				c->toSend.enqueue(b);
			// report the blob
			const char* r = b.unpack("S:");
			if(r)
				AddTraffic(r);
		}
		for(_client* &c : clients)
			if(!c->received.empty()){
				blob b = c->received.dequeue();
				server->toSend.enqueue(b);
				char temp[20];
				sprintf_s(temp, sizeof temp, "C%d:", c->id);
				const char* r = b.unpack(temp);
				if(r)
					AddTraffic(r);
			}
		Sleep(1);
	}
}
