#pragma once

// in client.cpp

class _client {
public:
	_client(int n, const char*url, unsigned short p);	// create and start
	~_client();									// stop and remove
	SafeQueue<blob> received;					// data from the external host
	SafeQueue<blob> toSend;						// data to send to the external host
	bool active(){ return bActive; }
	void getCounts(int& sent, int& recv){ sent=nSent; recv=nRecv; }
	int id{};

private:
	const char* url{};
	unsigned int port{};
	std::thread* cthread{};
	bool stop{};

	void worker();
	int nSent{};								// packet counters
	int nRecv{};
	bool bActive{};
};
