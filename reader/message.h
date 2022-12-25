#pragma once

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
//=================================================================================================
// heartbeat message
//=================================================================================================

class heartbeat : public message {
public:
	heartbeat(byte* &p, int &cb);			// read and create
	virtual ~heartbeat();					// clean up
	void print(FILE*);
public:
	char*		id;
	uint32_t	max_schema;
	char*		version;
	char*		revision;
};
//=================================================================================================
// status message
//=================================================================================================

class status : public message {
public:
	status(byte* &p, int &cb);			// read and create
	virtual ~status();					// clean up
	void print(FILE*);
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
//=================================================================================================
// decode message
//=================================================================================================

class decode : public message {
public:
	decode(byte* &p, int &cb);			// read and create
	virtual ~decode();					// clean up
	void print(FILE*);
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
