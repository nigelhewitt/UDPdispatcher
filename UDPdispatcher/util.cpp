//=================================================================================================
// util.cpp		all the non-specific routines
//=================================================================================================

#include "framework.h"

//=====================================================================================================
// handler to unpack Windows error codes into text  (Work in Wide so we can handle anything)
//=====================================================================================================

bool error(const char* id, DWORD err)
{
	char temp[500];
	int cb = sizeof temp;
	if(err == 0)
		err = GetLastError();
	strcpy_s(temp, sizeof temp, id);
	int n = (int)strlen(temp);
	sprintf_s(temp+n, sizeof temp-n, " Network Error %d: stop system?\r\n", err);
	n = (int)strlen(temp);
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), temp+n, sizeof temp-n, nullptr);
	for(int i=(int)strlen(temp); isspace(temp[i-1]); temp[--i]=0);		// remove \r\n
	AddTraffic(temp);
	return MessageBox(hMain, temp, "Network Error", MB_YESNO)==IDYES;
}

//=====================================================================================================
// simple unsigned to/from string routines
//=====================================================================================================

unsigned int atou(const char* c)
{
	unsigned int n=0;
	while(isspace(*c)) ++c;
	while(isdigit(*c)){
		n *= 10;
		n += *c++ -'0';
	}
	return n;
}
static void _utoa(char* &p, int cb, int n)
{
	if(n>=10)
		_utoa(p, cb-1, n/10);
	*p++ = (n %= 10) + '0';
}
void utoa(char* p, int cb, unsigned int n)
{
	_utoa(p, cb, n);
	*p = 0;
}
//=================================================================================================
// poll a socket for read or write to see if it will block
//=================================================================================================

bool WillNotBlock(SOCKET socket, SKT_MODE mode)
{
	WSAPOLLFD skt{};					// check if the socket can read/write without blocking
	skt.fd = socket;
	skt.events = mode==SKT_READ ? POLLRDNORM : POLLWRNORM;
	skt.revents = 0;
	return WSAPoll(&skt, 1, 0)>0;
}
