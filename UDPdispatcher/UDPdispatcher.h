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
bool error(DWORD err=0);						// returns true if the user select stop
unsigned int atou(const char* c);
void utoa(char* p, int cb, unsigned int n);
