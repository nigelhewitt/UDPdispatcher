#pragma once
//=================================================================================================
//		blob.h		definition of the packed UDP format stuff
//=================================================================================================

// the definition is in their file

class blob {
public:
	blob(){}							// default
	blob(const char* text, int cb){		// save the byte stream
		length = cb;
		data   = new byte[length+1];
		memcpy(data, text, length);
		data[length] = 0;				// the debugger loves to show it as a null terminated string and all goes wrong on me
	}
	blob(const blob& b){				// smart copy
		length = b.length;
		data   = new byte[length+1];
		memcpy(data, b.data, length);
		data[length] = 0;
	}
	~blob(){
		delete[] data;
	}
	const char* unpack(const char* prefix);		// blob to readable

	byte* data{};
	int	  length{};
	friend class _server;
};
