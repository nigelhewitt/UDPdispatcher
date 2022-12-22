//=================================================================================================
// blob.cpp		deal with the UDP packed encoding
//=================================================================================================

#include "framework.h"

DWORD swap(DWORD d)
{
	union { DWORD d; byte b[4]; } in, out;
	in.d = d;
	out.b[0] = in.b[3];
	out.b[1] = in.b[2];
	out.b[2] = in.b[1];
	out.b[3] = in.b[0];
	return out.d;
}

const char* b2h(byte b)
{
	static char temp[4]="xx ";
	byte bh = b>>4;
	if(bh>9) temp[0] = bh-10+'A';
	else	 temp[0] = bh+'0';
	b &= 0xf;
	if(b>9)  temp[1] = b-10+'A';
	else	 temp[1] = b+'0';
	return temp;
}
const char* b2a(byte b)
{
	static char temp[2]=" ";
	temp[0] = b;
	return temp;
}
const char* dump(const byte* p, int cb)
{
	static char temp[3000];
	temp[0] = 0;
	while(cb){
		int n=16, i=0;
		if(n>cb) n = cb;
		while(i<n)
			strcat_s(temp, sizeof temp, b2h(p[i++]));
		while(i<16)
			strcat_s(temp, sizeof temp, "   "), ++i;
		strcat_s(temp, sizeof temp, "  ");
		i = 0;
		while(i<n)
			strcat_s(temp, sizeof temp, b2a(p[i++]));
		p  += n;
		cb -= n;
		strcat_s(temp, sizeof temp, "\r\n");
	}
	return temp;
}
const char* blob::unpack(const char *prefix)
{
	static char temp[500];

	if(length<8){
		sprintf_s(temp, sizeof temp, "%s SHORT %d", prefix, length);
		return temp;
	}

	if(swap(*(DWORD*)data)!=0xadbccbda){
		sprintf_s(temp, sizeof temp, "%s BAD %d", prefix, length);
		return temp;
	}
	if(length>64){
		sprintf_s(temp, sizeof temp, "%s OK %d", prefix, length);
//		strcpy_s(temp, sizeof temp, (const char*)&data[44+8]);
		return temp;
	}
	return nullptr;
}