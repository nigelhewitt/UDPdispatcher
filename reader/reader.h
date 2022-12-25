#pragma once

#include <stdint.h>
#include <cstdio>
#include <conio.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <string.h>

#include "message.h"

typedef uint8_t byte;

// in unpack.cpp
class message;
message* unpack(byte* &p, int &cb);
void dump(FILE* f, void* buffer, int cb);
