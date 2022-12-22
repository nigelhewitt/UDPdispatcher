//=================================================================================================
// util.cpp		all the non-specific routines
//=================================================================================================

#include "framework.h"

//=====================================================================================================
// handler to unpack Windows error codes into text  (Work in Wide so we can handle anything)
//=====================================================================================================

bool error(DWORD err)
{
	char temp[200];
	int cb = sizeof temp;
	if(err == 0)
		err = GetLastError();
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), temp, sizeof temp, nullptr);
	for(int i=(int)strlen(temp); isspace(temp[i-1]); temp[--i]=0);		// remove \r\n
	AddTraffic(temp);
	return MessageBox(hMain, temp, "System Error: stop system?", MB_YESNO)==IDYES;
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

