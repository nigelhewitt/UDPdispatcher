// unpack.cpp
#include <stdint.h>
#include <cstdio>
#include <string.h>


typedef uint8_t byte;

uint32_t PK_uint32(byte* &p, int &cb)
{
	if(cb<4) throw 1;
	cb -= 4;
	union { byte b[4]; uint32_t u; } x;
	x.b[3] = *p++;
	x.b[2] = *p++;
	x.b[1] = *p++;
	x.b[0] = *p++;
	return x.u;
}

uint64_t PK_uint64(byte* &p, int &cb)
{
	if(cb<8) throw 2;
	cb -= 8;
	union { byte b[8]; uint64_t u; } x;
	x.b[7] = *p++;
	x.b[6] = *p++;
	x.b[5] = *p++;
	x.b[4] = *p++;
	x.b[3] = *p++;
	x.b[2] = *p++;
	x.b[1] = *p++;
	x.b[0] = *p++;
	return x.u;
}
bool PK_bool(byte* &p, int &cb)
{
	if(cb<1) throw 3;
	--cb;
	return *p++!=0;
}
uint8_t PK_uint8(byte* &p, int &cb)
{
	if(cb<1) throw 4;
	--cb;
	return *p++;
}

char* PK_utf8(byte* &p, int &cb)
{
	if(cb<4) throw 5;
	uint32_t n = PK_uint32(p, cb);		// does its own cb-=4
	if(n==0xffffffff){
		char *s = new char[5];
		strcpy_s(s, 5, "none");
		return s;
	}
	if(cb<(int)n) throw 6;
	cb -= n;
	char* s = new char[n+1];
	for(int i=0; i<(int)n; ++i)
		s[i] = *p++;
	s[n] = 0;
	return s;
}
double PK_double(byte* &p, int &cb)
{
	if(cb<8) throw 7;
	cb -= 8;
	union { byte b[8]; double d; } x;
	x.b[7] = *p++;
	x.b[6] = *p++;
	x.b[5] = *p++;
	x.b[4] = *p++;
	x.b[3] = *p++;
	x.b[2] = *p++;
	x.b[1] = *p++;
	x.b[0] = *p++;
	return x.d;
}
//=================================================================================================
// basic message class
//=================================================================================================

class message {
public:
	message(){}
	virtual ~message(){}
	virtual void print(FILE*);
public:
	uint32_t schema{};
	uint32_t type{};

};
void message::print()
{
	fprintf(fh, "data block:    %d bytes\n", cb);
	fprintf(fh, "schema:        %d\n", schema);
}
//=================================================================================================
// heartbeat message
//=================================================================================================

class heartbeat : public message {
public:
	heartbeat(byte* &p, int &cb);			// read and create
	virtual ~heartbeat();					// clean up
	virtual void print(FILE*);
public:
	char*		id;
	uint32_t	max_schema;
	char*		version;
	char*		revision;
};

heartbeat::heartbeat(FILE* fh, byte* &p, int &cb)
{
	id			= PK_utf8(p, cb);
	max_schema	= PK_uint32(p, cb);
	version		= PK_utf8(p, cb);
	revision	= PK_utf8(p, cb);
}
void HEATBEAT::prinf(FILE* fh)
{
	base::print(FILE* fh);
	fprintf(fh, "type:          heartbeat\n");
	fprintf(fh, "id:            %s\n", id);
	fprintf(fh, "max schema:    %u\n", max_schema);
	fprintf(fh, "version:       %s\n", version);
	fprintf(fh, "revision:      %s\n", revision);
}
HEATBEAT::~HEATRBEAT()
{
	delete[] id;
	delete[] version;
	delete[] revision;
}
//=================================================================================================
// status message
//=================================================================================================

class status:: public message {
public:
	status(byte* &p, int &cb);			// read and create
	virtual ~status();					// clean up
	virtual void print(FILE*);
public:
	char*		id{};
	uint64_t	dial{};
	char*		mode{};
	char*		DXcall{};
	char*		report{};
	char*		tx_mode{};
	bool		tx_enabled{};
	bool		transmitting{};
	bool		decoding{};
	uint32_t	rx_df{};
	uint32_t	tx_df{};
	char*		de_call{};
	char*		de_grid{};
	char*		dx_grid{};
	bool		tx_watchdog{};
	char*		sub_mode{};
	bool		fast_mode{};
	uint32_t	so_mode{};
	uint32_t	fr_tol{};
	uint32_t	tr_period{};
	char*		config_name{};
	char*		tx_message{};
};
status::status(byte* &p, int &cb)
{
	id				= PK_utf8(p, cb);
	dial			= PK_uint64(p, cb);
	mode			= PK_utf8(p, cb);
	DXcall			= PK_utf8(p, cb);
	report			= PK_utf8(p, cb);
	tx_mode			= PK_utf8(p, cb);
	tx_enabled		= PK_bool(p, cb);
	transmitting	= PK_bool(p, cb);
	decoding		= PK_bool(p, cb);
	rx_df			= PK_uint32(p, cb);
	tx_df			= PK_uint32(p, cb);
	de_call			= PK_utf8(p, cb);
	de_grid			= PK_utf8(p, cb);
	dx_grid			= PK_utf8(p, cb);
	tx_watchdog		= PK_bool(p, cb);
	sub_mode		= PK_utf8(p, cb);
	fast_mode		= PK_bool(p, cb);
	so_mode			= PK_uint8(p, cb);
	fr_tol			= PK_uint32(p, cb);
	tr_period		= PK_uint32(p, cb);
	config_name		= PK_utf8(p, cb);
	tx_message		= PK_utf8(p, cb);
}
void status::print(FILE* fh)
{
	base::print(FILE* fh);
	fprintf(fh, "type:          status\n");
	fprintf(fh, "id:            %s\n", id);
	fprintf(fh, "dial:          %I64u\n", dial);
	fprintf(fh, "mode:          %s\n", mode);
	fprintf(fh, "DX call:       %s\n", DXcall);
	fprintf(fh, "Report:        %s\n", report);
	fprintf(fh, "TX mode:       %s\n", tx_mode);
	fprintf(fh, "TX enabled:    %s\n", tx_enabled ? "true":"false");
	fprintf(fh, "Transmitting:  %s\n", transmitting ? "true":"false");
	fprintf(fh, "Decoding:      %s\n", decoding ? "true":"false");
	fprintf(fh, "Rx DF:         %u\n", rx_df);
	fprintf(fh, "Tx DF:         %u\n", tx_df);
	fprintf(fh, "DE call:       %s\n", de_call);
	fprintf(fh, "DE grid:       %s\n", de_grid);
	fprintf(fh, "DX grid:       %s\n", dx_grid);
	fprintf(fh, "TX watchdog:   %s\n", tx_watchdog ? "true":"false");
	fprintf(fh, "Su-mode:       %s\n", sub_mode);
	fprintf(fh, "Fast mode:     %s\n", fast_mode ? "true":"false");
	fprintf(fh, "SO mode:       %u\n", so_mode);
	fprintf(fh, "Freq Tol:      %u\n", fr_tol);
	fprintf(fh, "TR period:     %u\n", tr_period);
	fprintf(fh, "Conf name:     %s\n", config_name);
	fprintf(fh, "TX message:    %s\n\n", tx_message);
}
status::~status()
{
	delete[] id;
	delete[] mode;
	delete[] DXcall;
	delete[] report;
	delete[] tx_mode;
	delete[] de_call;
	delete[] de_grid;
	delete[] dx_grid;
	delete[] sub_mode;
	delete[] config_name;
	delete[] tx_message;
}
//=================================================================================================
// decode message
//=================================================================================================

class decode:: public message {
public:
	decode(byte* &p, int &cb);			// read and create
	virtual ~decode();					// clean up
	virtual void print(FILE*);
public:
	char*		id{};
	bool		val_new{};
	uint32_t	val_time{};
	uint32_t	snr{};
	double		deltat{};
	uint32_t	deltaf{};
	char*		mode{};
	char*		data{};
	bool		low_confidence{};
	bool		off_air{};
};
decode::decode(byte* &p, int &cb)
{
	id				= PK_utf8(p, cb);
	val_new			= PK_bool(p, cb);
	val_time		= PK_uint32(p, cb);
	snr				= PK_uint32(p, cb);
	deltat			= PK_double(p, cb);
	deltaf			= PK_uint32(p, cb);
	mode			= PK_utf8(p, cb);
	data			= PK_utf8(p, cb);
	low_confidence	= PK_bool(p, cb);
	off_air			= PK_bool(p, cb);
}
void decode::print(FILE* fh, byte* &p, int &cb)
{
	base::print(FILE* fh);
	fprintf(fh, "type:          decode\n");
	fprintf(fh, "id:            %s\n", id);
	fprintf(fh, "new:           %s\n", val_new ? "true":"false");
	fprintf(fh, "Time:          %u\n", val_time);
	fprintf(fh, "snr:           %u\n", snr);
	fprintf(fh, "delta time:    %f\n", deltat);
	fprintf(fh, "Delta freq:    %uHz\n", deltaf);
	fprintf(fh, "mode:          %s\n", mode);
	fprintf(fh, "message:       %s\n", data);
	fprintf(fh, "low conf:      %s\n", val_new ? "true":"false");
	fprintf(fh, "Off Air:       %s\n\n", off_air ? "true":"false");
}
decode::~decode()
{
	delete[] id;
	delete[] mode;
	delete[] data;
}
//=================================================================================================
// unpack message
//=================================================================================================

message* unpack(byte* &p, int &cb)
{
	try{
		uint32_t magic = PK_uint32(p, cb);
		if(magic != 0xadbccbda) throw 8;
		uint32_t schema = PK_uint32(p, cb);
		if(schema!=2 && schema!=3) throw 9;

		message* result{};
		uint32_t message_type  = PK_uint32(p, cb);
		switch(message_type){
		case 0:
			result = new heartbeat(fh, p, cb);
			break;
		case 1:
			result = new status(fh, p, cb);
			break;
		case 2:
			result = new decode(fh, p, cb);
			break;
		default:
			throw 10;
		}
		if(cb!=0){
			delete result;
			throw 11;
		}
		// finally put in the base class bits
		result->schema = schema;
		result->type = type;
		return result;
	}
	catch(int n){
		fprintf(fh, "BANG:          %d\n", n);
		return nullptr;
	}
	catch(...){
		fprintf(fh, "BANG:          unknown\n");
		return nullptr;
	}
}


void dump(FILE* f, void* buffer, int cb)
{
	char temp[100];
	uint8_t *buf = (uint8_t*)buffer;
	for(int i=0; i<cb; i+=16){
		int n = cb-i, j;
		if(n>16) n=16;
		sprintf_s(temp, sizeof temp, "\r\n%04X ", i);
		for(j=0; j<n; j++)
			sprintf_s(temp+7+j*3, sizeof temp-7-j*3, "%02X ", buf[i+j]);
		for( ;j<16; j++)
			strcpy_s(temp+7+j*3, sizeof temp-7-j*3, "   ");
		strcpy_s(temp+55, sizeof temp-55, "   ");
		for(j=0; j<n; j++){
			uint8_t c= buf[i+j];
			if(c<0x20 || c>=0x7f) c = ' ';
			temp[58+j*2] = c;
			temp[59+j*2] = ' ';
		}
		temp[57+j*2] = '\n';		// j was incremented offset back, less the last blank
		fwrite(temp, 1, 57+j*2, f);
	}
	fwrite("\n\n", 1, 1, f);
}
