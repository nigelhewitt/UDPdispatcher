#pragma once

class _dispatcher {
public:
	_dispatcher();
	~_dispatcher();
	void getCounts(int& sSent, int& sRecv, int& cSent, int& cRecv, int& nClient);
private:
	bool stop{};
	std::thread* dthread{};
	void worker();

	_server *server{};
	std::vector<_client*> clients{};
};
