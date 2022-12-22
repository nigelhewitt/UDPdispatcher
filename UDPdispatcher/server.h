#pragma once
// in server.cpp

class _server {
public:
	_server(unsigned short p=0);			// create and start
	~_server();								// stop and remove
	SafeQueue<blob> received;				// data from the external host
	SafeQueue<blob> toSend;					// data to send to the external host
	void getCounts(int& sent, int& recv){ sent=nSent; recv=nRecv; }
private:
	unsigned int port{};
	std::thread* sthread{};
	bool stop{};
	int nSent{};							// packet counters
	int nRecv{};

	void worker();
};
