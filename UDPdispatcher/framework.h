// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

// set the SDK version we want (default windows)
#include "targetver.h"

// Windows Header Files
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <commctrl.h>
#include <commdlg.h>

// C RunTime Header Files
#include <cstdio>
#include <cstdlib>
#include <direct.h>
#include <shlobj_core.h>

// std:: library header files
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

// Project specific header files
#include "resource.h"
#include "safequeue.h"
#include "config.h"
#include "blob.h"
#include "client.h"
#include "server.h"
#include "dispatcher.h"
#include "UDPdispatcher.h"

// modules for the linker to include
#pragma comment(lib,"ws2_32.lib") 
