#pragma once
//=================================================================================================
// configuration system
//=================================================================================================

struct _config {
	struct server_details {
		unsigned short port{};
	} server;
	struct client_details {
		client_details(){}
		client_details(const char* u, unsigned short p){ if(u) url=_strdup(u); else url=nullptr; port = p; }
		const char* url{};
		unsigned short port{};
	};
	std::vector<client_details> clients;
};

extern _config config;
void readConfiguration();
void writeConfiguration();
DWORD readINI(const char* section, const char* item, const char* def, char* buffer, int cb);
BOOL writeINI(const char* section, const char* item, const char* value);
