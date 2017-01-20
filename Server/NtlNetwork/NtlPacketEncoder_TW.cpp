#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include "StdAfx.h"
#include "NtlPacketEncoder_TW.h"
#include "NtlPacket.h"
#include "NtlError.h"







static const unsigned int send_header_seed	= 0xbbcceeff;
static const unsigned int send_body_seed	= 0xddbbccaa;
static const unsigned int recv_header_seed	= 0xffeeccbb;
static const unsigned int recv_body_seed	= 0xaabbccdd;

static const unsigned int checksum_default = 0x7f;


/* 8bit CRC pre-generated table
 * seems to be the same as described in
 * http://www.zotax-gps.ru/UserFiles/inst/file/uzi-algoritm-crc.pdf 
 */
static const unsigned char crc8_table[] =
{
    0,  94, 188, 226,  97,  63, 221, 131, 194, 156, 126,  32, 163, 253,  31,  65,
  157, 195,  33, 127, 252, 162,  64,  30,  95,   1, 227, 189,  62,  96, 130, 220,
   35, 125, 159, 193,  66,  28, 254, 160, 225, 191,  93,   3, 128, 222,  60,  98,
  190, 224,   2,  92, 223, 129,  99,  61, 124,  34, 192, 158,  29,  67, 161, 255,
   70,  24, 250, 164,  39, 121, 155, 197, 132, 218,  56, 102, 229, 187,  89,   7,
  219, 133, 103,  57, 186, 228,   6,  88,  25,  71, 165, 251, 120,  38, 196, 154,
  101,  59, 217, 135,   4,  90, 184, 230, 167, 249,  27,  69, 198, 152, 122,  36,
  248, 166,  68,  26, 153, 199,  37, 123,  58, 100, 134, 216,  91,   5, 231, 185,
  140, 210,  48, 110, 237, 179,  81,  15,  78,  16, 242, 172,  47, 113, 147, 205,
   17,  79, 173, 243, 112,  46, 204, 146, 211, 141, 111,  49, 178, 236,  14,  80,
  175, 241,  19,  77, 206, 144, 114,  44, 109,  51, 209, 143,  12,  82, 176, 238,
   50, 108, 142, 208,  83,  13, 239, 177, 240, 174,  76,  18, 145, 207,  45, 115,
  202, 148, 118,  40, 171, 245,  23,  73,   8,  86, 180, 234, 105,  55, 213, 139,
   87,   9, 235, 181,  54, 104, 138, 212, 149, 203,  41, 119, 244, 170,  72,  22,
  233, 183,  85,  11, 136, 214,  52, 106,  43, 117, 151, 201,  74,  20, 246, 168,
  116,  42, 200, 150,  21,  75, 169, 247, 182, 232,  10,  84, 215, 137, 107,  53
};




CNtlPacketEncoder_TW::CNtlPacketEncoder_TW(bool is_client) :
is_client_(is_client)
{
	if (is_client_)
	{
		rand_send_header = recv_header_seed;
		rand_send_body = recv_body_seed;
		rand_recv_header = send_header_seed;
		rand_recv_body = send_body_seed;
	}
	else
	{
		rand_send_header = send_header_seed;
		rand_send_body = send_body_seed;
		rand_recv_header = recv_header_seed;
		rand_recv_body = recv_body_seed;
	}

}



CNtlPacketEncoder_TW::~CNtlPacketEncoder_TW()
{
}



static int
encrypt_(char *buf, unsigned int buf_size, unsigned int key, unsigned int *checksum)
{
	uint32_t *p = (uint32_t *) buf;
	uint8_t *r;
	unsigned int max;

	if (checksum != NULL)
	{
		*checksum = checksum_default;
	}

	if (buf == NULL || buf_size == 0)
	{
		return 0;
	}

	/* xor every 4 bytes */
	max = buf_size / sizeof(uint32_t);

	for (unsigned int n = 0; n < max; n++)
	{
		if (checksum != NULL)
		{
			*checksum = crc8_table[*checksum ^ (uint8_t) *p];
		}

		*p ^= key;

		p++;
	}

	/* xor the remaining bytes */
	r = (uint8_t *) p;
	max = buf_size % sizeof(uint32_t);

	for (unsigned int n = 0; n < max; n++)
	{
		if (checksum != NULL)
		{
			*checksum = crc8_table[*checksum ^ *r];
		}

		*r ^= (uint8_t) key;

		r++;
	}

	return buf_size;
}


static int
decrypt_(char *buf, unsigned int buf_size, unsigned int key, unsigned int *checksum)
{
	uint32_t *p = (uint32_t *) buf;
	uint8_t *r;
	unsigned int max;

	if (checksum != NULL)
	{
		*checksum = checksum_default;
	}

	if (buf == NULL || buf_size == 0)
	{
		return 0;
	}

	/* xor every 4 bytes */
	max = buf_size / sizeof(uint32_t);

	for (unsigned int n = 0; n < max; n++)
	{
		*p ^= key;

		if (checksum != NULL)
		{
			*checksum = crc8_table[*checksum ^ (uint8_t) *p];
		}

		p++;
	}

	/* xor the remaining bytes */
	r = (uint8_t *) p;
	max = buf_size % sizeof(uint32_t);

	for (unsigned int n = 0; n < max; n++)
	{
		*r ^= (uint8_t) key;

		if (checksum != NULL)
		{
			*checksum = crc8_table[*checksum ^ *r];
		}

		r++;
	}

	return buf_size;
}


int
CNtlPacketEncoder_TW::encrypt(CNtlPacket *packet, bool send)
{
	unsigned int header_key, body_key;
	unsigned int checksum = 0;

	if (packet == NULL)
	{
		return -1;
	}

	if (send)
	{
		header_key = rand_send_header.Generate();
		body_key = rand_send_body.Generate();
	}
	else
	{
		header_key = rand_recv_header.Generate();
		body_key = rand_recv_body.Generate();
	}

	if (!packet->GetPacketHeader()->bEncrypt)
	{
		encrypt_((char*)packet->GetPacketData(), packet->GetPacketDataSize(), body_key, &checksum);
		//packet->(checksum);
		encrypt_((char*)packet->GetPacketHeader(), packet->GetHeaderSize(), header_key, NULL);
		packet->GetPacketHeader()->bEncrypt = true;
		packet->GetPacketData();
	}

	return 0;
}


int
CNtlPacketEncoder_TW::decrypt(CNtlPacket *packet, bool send)
{
	unsigned int header_key, body_key;
	unsigned int checksum = 0;

	if (packet == NULL)
	{
		return -1;
	}

#if 0
	if (send)
	{
		header_key = rand_send_header.Generate();
		body_key = rand_send_body.Generate();
	}
	else
	{
		header_key = rand_recv_header.Generate();
		body_key = rand_recv_body.Generate();
	}
#endif

	/* header only decryption is allowed */
	if (packet->GetPacketHeader()->bEncrypt == true)
	{
		if (send)
		{
			header_key = rand_send_header.Generate();
		}
		else
		{
			header_key = rand_recv_header.Generate();
		}

		decrypt_((char*)packet->GetPacketHeader(), packet->GetHeaderSize(), header_key, NULL);
		packet->GetPacketHeader()->bEncrypt = false;
	}
	else
	{
		std::cout << "Skipping Header Decryption" << std::endl;
	}

	unsigned int body_size = packet->GetPacketDataSize();

	if (packet->GetPacketData())
	{
		if (body_size > 0)
		{
			if (send)
			{
				body_key = rand_send_body.Generate();
			}
			else
			{
				body_key = rand_recv_body.Generate();
			}

			decrypt_((char*)packet->GetPacketData(), body_size, body_key, &checksum);

		///*	if (checksum != packet->GetPacketHeader()->byChecksum)
		//	{
		//		std::cout << " decrypt checksum failed" << std::endl;
		//		return -1;
		//	}*/
		//	else
		//	{
		//		std::cout << " decrypt checksum Matched" << std::endl;
		//	}

			//packet->body_encrypted_set(true);
		}
		else
		{
			std::cout << "no body to decrypt" << std::endl;//dbo_log(DBO_LOG_LEVEL_DEBUG, "%s: no body to decrypt\n", __FUNCTION__);
		}
	}
	else
	{
		std::cout << "body not encrypted" << std::endl; //dbo_log(DBO_LOG_LEVEL_DEBUG, "%s: body not encrypted\n", __FUNCTION__);
	} 

	return 0;
}


