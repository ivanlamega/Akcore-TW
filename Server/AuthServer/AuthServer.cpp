//-----------------------------------------------------------------------------------
//		Auth Server by Daneos @ Ragezone 
//-----------------------------------------------------------------------------------

#include "stdafx.h"
#include "AuthServer.h"

#include "NtlSfx.h"
#include "NtlFile.h"

#include "NtlPacketUA.h"
#include "NtlPacketAU.h"
#include "ResultCode.h"

#include <iostream>
#include <map>
#include <list>

using namespace std;
bool isHandshaken = false;
CClientSession * client;
//-----------------------------------------------------------------------------------
CClientSession::~CClientSession()
{
	//NTL_PRINT(PRINT_APP, "CClientSession Destructor Called");
}


int CClientSession::OnAccept()
{
	NTL_PRINT( PRINT_APP, "%s", __FUNCTION__ );
	CAuthServer * app = (CAuthServer*)NtlSfxGetApp();
	CNtlPacket packet(0x06);
	unsigned char buf[] = { 0x03, 0x00, 0xac, 0x86, 0xf5, 0x74 };
	packet.SetPacket(buf, 0x06);
	packet.GetPacketHeader()->bEncrypt = true;
	PushHandshakePacket(&packet);	

	return NTL_SUCCESS;
}


void CClientSession::OnClose()
{
	NTL_PRINT( PRINT_APP, "%s", __FUNCTION__ );
	CAuthServer * app = (CAuthServer*) NtlSfxGetApp();
	isHandshaken = false;
}

int CClientSession::OnDispatch(CNtlPacket * pPacket)
{
	CAuthServer * app = (CAuthServer*) NtlSfxGetApp();
	//CNtlPacketEncoder_RandKey crypt;

	//CNtlPacket* dPacket = (CNtlPacket*)crypt.RxDecrypt(pPacket);
	sNTLPACKETHEADER * pHeader = (sNTLPACKETHEADER *)pPacket->GetPacketData();
	//printf("~~~ opcode %i received ~~~ \n", pHeader->wOpCode);
	//CPacketTrace* pTrace = app->packetTrace->GetInstance();
//	const char* pcName = pTrace->GetPacketName(pHeader->wOpCode);
	if (pHeader->wOpCode > 16)
	NTL_PRINT(PRINT_SYSTEM, "%s [%u] Size[%u]", NtlGetPacketName_UA(pHeader->wOpCode) , pHeader->wOpCode, sizeof(pPacket));
	
	switch( pHeader->wOpCode )
	{
		case 0x04:
		{
			if (isHandshaken == false)
			{
				CNtlPacket packet2(0x22);
				unsigned char buf2[] = { 0x10, 0x00, 0x84, 0xfb, 0x48, 0xf4, 0x8e, 0x5a, 0xb6, 0x67, 0xe2, 0x3d, 0x6e, 0x14, 0xb4, 0xa3, 0xc3, 0x24, 0x9e, 0x5f, 0xe3, 0xd1, 0xd5, 0x88, 0x10, 0x0d, 0x68, 0x4f, 0x3b, 0xa5, 0xed, 0x37, 0xed, 0x4a };
				packet2.SetPacket(buf2, 0x22);
				packet2.GetPacketHeader()->bEncrypt = false;
				PushHandshakePacket(&packet2);
				isHandshaken = true;
				client = this;
			}

		}
			break;
		case UA_LOGIN_REQ:{}break;
		case UA_LOGIN_REQ_TAIWAN_CT:
		{
			CClientSession::SendCharLogInReq(pPacket, app);
		}
			break;
		case UA_LOGIN_DISCONNECT_REQ:
		{
			CClientSession::SendLoginDcReq(pPacket);
		}
		break;

		default:
		{
			return CNtlSession::OnDispatch(pPacket);
		}
	}

	return NTL_SUCCESS;
}



//-----------------------------------------------------------------------------------
//		AuthServerMain
//-----------------------------------------------------------------------------------
int AuthServerMain(int argc, _TCHAR* argv[])
{
	CAuthServer app;
	CNtlFileStream traceFileStream;
	// LOG FILE
	int rc = traceFileStream.Create("authlog");
	if (NTL_SUCCESS != rc)
	{
		printf("log file CreateFile error %d(%s)", rc, NtlGetErrorMessage(rc));
		return rc;
	}
	app.packetTrace->CreateInstance();
	// CHECK INI FILE AND START PROGRAM
	NtlSetPrintStream(traceFileStream.GetFilePtr());
	NtlSetPrintFlag(PRINT_APP | PRINT_SYSTEM);

	rc = app.Create(argc, argv, ".\\Server.ini");
	if (NTL_SUCCESS != rc)
	{
		NTL_PRINT(PRINT_APP, "Server Application Create Fail %d(%s)", rc, NtlGetErrorMessage(rc));
		return rc;
	}
	
	// CONNECT TO MYSQL
	app.db = new MySQLConnWrapper;
	app.db->setConfig(app.GetConfigFileHost(), app.GetConfigFileUser(), app.GetConfigFilePassword(), app.GetConfigFileDatabase());
	try
	{
		app.db->connect();
		printf("Connected to database server.\n\r");
	}
	catch (exception e)
	{
		printf("couldn't connect to database server ErrID:%s\n\r", e.what());
	}
	try
	{
		app.db->switchDb(app.GetConfigFileDatabase());
	}
	catch (exception e)
	{
		printf("Couldn't switch database to %s Error:%s\n\r", app.GetConfigFileDatabase(), e.what());
	}
	app.Start();
	Sleep(500);
	std::cout << "\n\n" << std::endl;
	std::cout << "\t  ____                              ____        _ _ " << std::endl;
	std::cout << "\t |  _ \\ _ __ __ _  __ _  ___  _ __ | __ )  __ _| | |" << std::endl;
	std::cout << "\t | | | | '__/ _` |/ _` |/ _ \\| '_ \\|  _ \\ / _` | | |" << std::endl;
	std::cout << "\t | |_| | | | (_| | (_| | (_) | | | | |_) | (_| | | |" << std::endl;
	std::cout << "\t |____/|_|  \\__,_|\\__, |\\___/|_| |_|____/ \\__,_|_|_|" << std::endl;
	std::cout << "\t                  |___/                             " << std::endl;
	std::cout << "\t______           Dragon Ball Tw Emulator	______\n\n" << std::endl;
	std::cout << "		Contributors - AKCORE, Xanu, Nicolas321				" << std::endl;
	
	
	app.WaitForTerminate();
	

	return 0;
}
