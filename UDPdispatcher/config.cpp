//=================================================================================================
// configuration system
//=================================================================================================

#include "framework.h"

//=====================================================================================================
// ini file stuff
//=====================================================================================================

char dataFolder[MAX_PATH]{};
static char iniFile[MAX_PATH]{};

bool readINI()				// returns true if it set things up new
{
	if(iniFile[0]==0){
		bool newLoad = false;
		PWSTR appdata{};
		if(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, nullptr, &appdata) == S_OK){
		    wcstombs_s(nullptr, dataFolder, sizeof dataFolder, appdata, MAX_PATH);

			if(!FileExists(dataFolder))
				exit(1);
			strcat_s(dataFolder, sizeof dataFolder, "\\Nigsoft");
			if(!FileExists(dataFolder))
				(void)_mkdir(dataFolder);

			if(!FileExists(dataFolder))
				exit(1);
			strcat_s(dataFolder, sizeof dataFolder, "\\UDPdispatcher");
			if(!FileExists(dataFolder)){
				newLoad = true;
				(void)_mkdir(dataFolder);
			}
		}
		else
			exit(99);

		strcpy_s(iniFile, sizeof iniFile, dataFolder);
		strcat_s(iniFile, sizeof iniFile, "\\config.edc");
		return newLoad;
	}
	return false;
}
DWORD readINI(const char* section, const char* item, const char* def, char* buffer, int cb)
{
	readINI();
	return GetPrivateProfileString(section, item, def, buffer, cb, iniFile);
}
BOOL writeINI(const char* section, const char* item, const char* value)
{
	readINI();
	return WritePrivateProfileString(section, item, value, iniFile);
}
//=====================================================================================================
// read/save the config.edc file in ini file format
//=====================================================================================================

_config config;		// instance the data structure globally

void readConfiguration()
{
	readINI();

	// read the server
	char temp[20];
	readINI("server", "port", "2237", temp, sizeof temp);
	config.server.port = atou(temp);

	// read the clients
	char name[20]="C", url[MAX_PATH];
	unsigned int n = 0;
	while(true){
		utoa(name+1, sizeof name-1, ++n);
		if(readINI("clientURL", name, "", url, sizeof url)==0) break; // no entry
		readINI("clientPort", name, "", temp, sizeof temp);
		_config::client_details cd{};
		cd.url  = _strdup(url);
		cd.port = atou(temp);
		config.clients.push_back(cd);
	}
}
void writeConfiguration()
{
	readINI();
	// write the server
	char temp[20];
	utoa(temp, sizeof temp, config.server.port);
	writeINI("server", "port", temp);

	// write the clients
	char name[20]="C";	
	unsigned int n = 0;
	for(_config::client_details s : config.clients){
		utoa(name+1, sizeof name-1, ++n);
		writeINI("clientURL", name, s.url);
		utoa(temp, sizeof temp, s.port);
		writeINI("clientPort", name, temp);
	}
	// remove any extras
	while(true){
		utoa(name+1, sizeof name-1, ++n);
		if(readINI("clientURL", name, "", temp, sizeof temp)==0) break;		 // ie: entry does not exist
		writeINI("clientURL", name, nullptr);
		writeINI("slientrPort", name, nullptr);
	}
}
