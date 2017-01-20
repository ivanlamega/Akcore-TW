//-----------------------------------------------------------------------------------
//		Char Server by Daneos @ Ragezone 
//-----------------------------------------------------------------------------------

#include "stdafx.h"
#include "CharServer.h"

#include "NtlSfx.h"
#include "NtlFile.h"

#include "NtlPacketUC.h"
#include "NtlPacketCU.h"
#include "ResultCode.h"

#include <iostream>
#include <map>
#include <list>

using namespace std;

//-----------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------
CClientSession::~CClientSession()
{
	//NTL_PRINT(PRINT_APP, "CClientSession Destructor Called");
}


int CClientSession::OnAccept()
{
	//NTL_PRINT( PRINT_APP, "%s", __FUNCTION__ );
	this->isHandShaken = false;
	CNtlPacket packet(0x06);
	unsigned char buf[] = { 0x03, 0x00, 0xac, 0x86, 0xf5, 0x74 };
	packet.SetPacket(buf, 0x06);
	packet.GetPacketHeader()->bEncrypt = true;
	PushHandshakePacket(&packet);
	return NTL_SUCCESS;

}


void CClientSession::OnClose()
{
	//NTL_PRINT( PRINT_APP, "%s", __FUNCTION__ );
	CCharServer * app = (CCharServer*)NtlSfxGetApp();
	this->isHandShaken = false;
}


int CClientSession::OnDispatch(CNtlPacket * pPacket)
{
	CCharServer * app = (CCharServer*) NtlSfxGetApp();
	sNTLPACKETHEADER * pHeader = (sNTLPACKETHEADER *)pPacket->GetPacketData();
	if (pHeader->wOpCode != 1)
	//printf("~~~ opcode %i received ~~~ \n", pHeader->wOpCode);
	if (pHeader->wOpCode > 16)
	NTL_PRINT(PRINT_SYSTEM, "%s [%u] Size[%u]", NtlGetPacketName_UC(pHeader->wOpCode), pHeader->wOpCode, sizeof(pPacket));
	switch( pHeader->wOpCode )
	{
	case 0x04:
	{
		if (this->isHandShaken == false)
		{
			CNtlPacket packet2(0x22);
			unsigned char buf2[] = { 0x10, 0x00, 0x84, 0xfb, 0x48, 0xf4, 0x8e, 0x5a, 0xb6, 0x67, 0xe2, 0x3d, 0x6e, 0x14, 0xb4, 0xa3, 0xc3, 0x24, 0x9e, 0x5f, 0xe3, 0xd1, 0xd5, 0x88, 0x10, 0x0d, 0x68, 0x4f, 0x3b, 0xa5, 0xed, 0x37, 0xed, 0x4a };
			packet2.SetPacket(buf2, 0x22);
			packet2.GetPacketHeader()->bEncrypt = false;
			PushHandshakePacket(&packet2);
			this->isHandShaken = true;
		}

	}
		break;
	case UC_LOGIN_REQ:
	{
		CClientSession::SendCharServerReq(pPacket);
	}
		break;
	case UC_CHARACTER_SERVERLIST_REQ:
	{
		//printf("UC CHAR SERVER LIST REQ");
		//CClientSession::SendServerListOneReq(pPacket);
		CClientSession::SendServerListReq(pPacket, app);
	}
	break;
	case UC_CHARACTER_SERVERLIST_ONE_REQ:
	{
		CClientSession::SendServerListOneReq(pPacket);
	}
		break;
	case UC_CHARACTER_ADD_REQ:
	{
		CClientSession::SendCharCreateReq(pPacket, app);
	}
		break;
	case UC_CHARACTER_DEL_REQ:
	{
		CClientSession::SendCharDeleteReq(pPacket, app);
	}
		break;
	case UC_CHARACTER_SELECT_REQ:
	{
		CClientSession::SendCharSelectReq(pPacket);
	}
	break;
	case UC_CHARACTER_EXIT_REQ:
	{										
		CClientSession::SendCharExitReq(pPacket);
	}
		break;
	case UC_CHARACTER_LOAD_REQ:
	{
		CClientSession::SendCharLoadReq(pPacket, app);
	}
	break;
	case UC_CHARACTER_DEL_CANCEL_REQ:
	{
		CClientSession::SendCharDeleteCancelReq(pPacket, app);
	}
	break;
	case UC_CONNECT_WAIT_CHECK_REQ:
	{
		CClientSession::SendCharWaitCheckReq(pPacket);
	}
		break;
	case UC_CONNECT_WAIT_CANCEL_REQ:
	{
		CClientSession::SendCancelWaitReq(pPacket);
	}
	break;
	case UC_CHARACTER_RENAME_REQ:
	{
		CClientSession::SendCharRenameReq(pPacket, app);
	}
		break;
	
	case UC_CHAR_SERVERLIST_REQ:
	{
		//cout << "~~~~~~~~~~~~SERVER LIST REQUEST~~~~~~~~~~~~" << endl;
		CClientSession::SendServerListReq(pPacket, app);
		//CClientSession::SendCancelWaitReq(pPacket);
		//CClientSession::SendServerListOneReq(pPacket);
	}
		break;

	//case UC_CASHITEM_HLSHOP_REFRESH_REQ:
	//{
		//cout << "~~~~~~~~~~~~CASH SHOP REFRESH~~~~~~~~~~~~" << endl;
		//CClientSession::SendCancelWaitReq(pPacket);
	//}
		//break;
	default:
		return CNtlSession::OnDispatch( pPacket );
	}

	return NTL_SUCCESS;
}


//-----------------------------------------------------------------------------------
//		CharServerMain
//-----------------------------------------------------------------------------------
int CharServerMain(int argc, _TCHAR* argv[])
{
	CNtlFileStream traceFileStream;
	CCharServer app;
// LOG FILE
	int rc = traceFileStream.Create( "charlog" );
	if( NTL_SUCCESS != rc )
	{
		NTL_PRINT(PRINT_APP, "log file CreateFile error %d(%s)", rc, NtlGetErrorMessage( rc ) );
		return rc;
	}
// CHECK INT FILE
	NtlSetPrintStream( traceFileStream.GetFilePtr() );
	NtlSetPrintFlag( PRINT_APP | PRINT_SYSTEM );
	rc = app.Create(argc, argv, ".\\Server.ini");
	if( NTL_SUCCESS != rc )
	{
		NTL_PRINT(PRINT_APP, "Server Application Create Fail %d(%s)", rc, NtlGetErrorMessage(rc) );
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
	app.db2 = new MySQLConnWrapper;
	app.db2->setConfig(app.GetConfigFileHost(), app.GetConfigFileUser(), app.GetConfigFilePassword(), app.GetConfigFileDatabase());
	try
	{
		app.db2->connect();
		printf("Connected to database server.\n\r");
	}
	catch (exception e)
	{
		printf("couldn't connect to database server ErrID:%s\n\r", e.what());
	}
	try
	{
		app.db2->switchDb(app.GetConfigFileDatabase());
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
