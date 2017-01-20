#pragma once
#include <iostream>
#include <stdint.h>
#include "NtlPacketEncoder.h"
#include "dbo_random.h"
class CNtlPacketEncoder_TW : public CNtlPacketEncoder
{
public:

	CNtlPacketEncoder_TW(bool is_client = false);
	~CNtlPacketEncoder_TW(void);


public:

	/* send == false for recv */
	int encrypt(CNtlPacket *packet, bool send);
	
	int decrypt(CNtlPacket *packet, bool send);
	
private:


	bool is_client_;
	
	dbo_random rand_send_header;
	
	dbo_random rand_send_body;
	
	dbo_random rand_recv_header;
	
	dbo_random rand_recv_body;

};


