#pragma once

inline bool FileExists(const char* fn){ return GetFileAttributes(fn)!=0xffffffff; }

#define SIZEOF(A)	(sizeof A/sizeof A[0])	// number of elements in a compiled in array

// in UDPdispatcher.cpp
extern HINSTANCE hInstance;
extern HWND hMain;
extern HWND hTraffic;
void AddTraffic(const char* text);
void clearTraffic();

// routines in util.cpp
bool error(const char* id, DWORD err=0);		// returns true if the user selected OK to stop the system
unsigned int atou(const char* c);
void utoa(char* p, int cb, unsigned int n);

enum SKT_MODE { SKT_READ, SKT_WRITE };			// socket will not block check
bool WillNotBlock(SOCKET s, SKT_MODE mode);
