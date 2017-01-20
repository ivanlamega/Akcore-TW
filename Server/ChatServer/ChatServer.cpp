//-----------------------------------------------------------------------------------
//		Chat Server by Daneos @ Ragezone 
//-----------------------------------------------------------------------------------

#include "stdafx.h"
#include "ChatServer.h"


using namespace std;

//-----------------------------------------------------------------------------------
CClientSession::~CClientSession()
{
	//NTL_PRINT(PRINT_APP, "CClientSession Destructor Called");
}


int CClientSession::OnAccept()
{
	//NTL_PRINT( PRINT_APP, "%s", __FUNCTION__ );
	avatarHandle = AcquireSerialId();
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
	CChatServer * app = (CChatServer*) NtlSfxGetApp();
	app->RemoveUser(this->GetCharName().c_str());
}

int CClientSession::OnDispatch(CNtlPacket * pPacket)
{
	CChatServer * app = (CChatServer*) NtlSfxGetApp();

	sNTLPACKETHEADER * pHeader = (sNTLPACKETHEADER *)pPacket->GetPacketData();
	if(pHeader->wOpCode > 1) {
		//printf("~~~ opcode %i received ~~~ \n", pHeader->wOpCode);
		if (pHeader->wOpCode > 16)
		NTL_PRINT(PRINT_SYSTEM, "%s [%u] Size[%u]", NtlGetPacketName_UT(pHeader->wOpCode), pHeader->wOpCode, sizeof(pPacket));
	}
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
	case UT_ENTER_CHAT:
	{
		CClientSession::SendEnterChat(pPacket, app);
		CClientSession::SendFriendList(pPacket, app);
		CClientSession::SendLoadGuildInfo(pPacket, app);
		//CClientSession::SendLoadGuildMember(pPacket, app);
	}
		break;
	case UT_CHAT_MESSAGE_SAY:
	{
		CClientSession::SendSayReq(pPacket, app);
	}
		break;
	case UT_CHAT_MESSAGE_SHOUT:
	{
		printf("UT_CHAT_MESSAGE_SHOUT");
		CClientSession::SendShoutReq(pPacket, app);
	}
		break;
	case UT_CHAT_MESSAGE_WHISPER:
	{
		printf("UT_CHAT_MESSAGE_WHISPER");
		CClientSession::SendWhisperReq(pPacket, app);
	}
		break;
	case UT_CHAT_MESSAGE_FIND_PARTY:
	{
		printf("UT_CHAT_MESSAGE_FIND_PARTY");
		CClientSession::SendFindParty(pPacket, app);
	}
		break;
	case UT_CHAT_MESSAGE_PARTY:
	{
		printf("UT_CHAT_MESSAGE_PARTY");
		//CClientSession::SendPartyChatReq(pPacket, app);
	}
		break;
	case UT_CHAT_MESSAGE_GUILD:
	{
		printf("UT_CHAT_MESSAGE_GUILD");
		//CClientSession::SendGuildChatReq(pPacket, app);
	}
		break;
	case UT_CHAT_MESSAGE_TRADE:
	{
		printf("UT_CHAT_MESSAGE_TRADE");
		CClientSession::SendMessageTrade(pPacket, app);
	}
		break;
	case UT_CHAT_USER_ON_CHANNEL_UPDATE_NFY:
	{
		printf("UT_CHAT_USER_ON_CHANNEL_UPDATE_NFY");
		//	CClientSession::SendGuildChatReq(pPacket, app);
	}
		break;

	case UT_GUILD_DISBAND_REQ:
	{
		CClientSession::SendDisbandGuild(pPacket, app);
	}
		break;
	case UT_GUILD_DISBAND_CANCEL_REQ:
	{
		printf("UT_GUILD_DISBAND_CANCEL_REQ");
	}
		break;
	case UT_GUILD_RESPONSE_INVITATION:
	{
		printf("UT_GUILD_RESPONSE_INVITATION");
	}
		break;
	case UT_GUILD_LEAVE_REQ:
	{
		CClientSession::SendLeaveGuildReq(pPacket, app);
	}
		break;
	case UT_GUILD_KICK_OUT_REQ:
	{
		CClientSession::SendKickFromGuildReq(pPacket, app);
	}
		break;
	case UT_GUILD_APPOINT_SECOND_MASTER_REQ:
	{
		CClientSession::SendNewSecondGuildMaster(pPacket, app);
	}
		break;
	case UT_GUILD_DISMISS_SECOND_MASTER_REQ:
	{
		CClientSession::SendRemoveSecondGuildMaster(pPacket, app);
	}
		break;
	case UT_GUILD_CHANGE_GUILD_MASTER_REQ:
	{
		CClientSession::SendUpdateGuildMaster(pPacket, app);
	}
		break;
	case UT_CHAT_MESSAGE_PRIVATESHOP_BUSINESS:
	{
		printf("UT_CHAT_MESSAGE_PRIVATESHOP_BUSINESS");
	}
		break;
	case UT_FRIEND_ADD_REQ:
	{
		printf("UT_FRIEND_ADD_REQ");
		CClientSession::SendAddFriend(pPacket, app);
	}
		break;
	case UT_FRIEND_DEL_REQ:
	{
		printf("UT_FRIEND_DEL_REQ");
		CClientSession::SendDelFriend(pPacket, app);
	}
		break;
	case UT_FRIEND_MOVE_REQ:
	{
		printf("UT_FRIEND_MOVE_REQ");
		CClientSession::SendMoveFriend(pPacket, app);
	}
		break;
	case UT_FRIEND_BLACK_ADD_REQ:
	{
		printf("UT_FRIEND_BLACK_ADD_REQ");
		CClientSession::SendBlackListAdd(pPacket, app);
	}
		break;
	case UT_FRIEND_BLACK_DEL_REQ:
	{
		printf("UT_FRIEND_BLACK_DEL_REQ");
		CClientSession::SendBlackListDel(pPacket, app);
	}
		break;
	case UT_RANKBATTLE_RANK_LIST_REQ:
	{
		printf("UT_RANKBATTLE_RANK_LIST_REQ");
	}
		break;
	case UT_RANKBATTLE_RANK_FIND_CHARACTER_REQ:
	{
		printf("UT_RANKBATTLE_RANK_FIND_CHARACTER_REQ");
	}
		break;
	case UT_RANKBATTLE_RANK_COMPARE_DAY_REQ:
	{
		printf("UT_RANKBATTLE_RANK_COMPARE_DAY_REQ");
	}
		break;
	case UT_GUILD_CHANGE_NOTICE_REQ:
	{
		CClientSession::SendGuildChangeNotice(pPacket, app);
	}
		break;
	case UT_TMQ_RECORD_LIST_REQ:
	{
		printf("UT_TMQ_RECORD_LIST_REQ");
	}
		break;
	case UT_TMQ_MEMBER_LIST_REQ:
	{
		printf("UT_TMQ_MEMBER_LIST_REQ");
	}
		break;
	case UT_BUDOKAI_TOURNAMENT_INDIVIDUAL_LIST_REQ:
	{
		printf("UT_BUDOKAI_TOURNAMENT_INDIVIDUAL_LIST_REQ");
	}
		break;
	case UT_BUDOKAI_TOURNAMENT_INDIVIDUAL_INFO_REQ:
	{
		printf("UT_BUDOKAI_TOURNAMENT_INDIVIDUAL_INFO_REQ");
	}
		break;
	case UT_BUDOKAI_TOURNAMENT_TEAM_LIST_REQ:
	{
		printf("UT_BUDOKAI_TOURNAMENT_TEAM_LIST_REQ");
	}
		break;
	case UT_BUDOKAI_TOURNAMENT_TEAM_INFO_REQ:
	{
		printf("UT_BUDOKAI_TOURNAMENT_TEAM_INFO_REQ");
	}
		break;
	case UT_PETITION_CHAT_START_RES:
	{
		printf("UT_PETITION_CHAT_START_RES");
	}
		break;
	case UT_PETITION_CHAT_USER_SAY_REQ:
	{
		printf("UT_PETITION_CHAT_USER_SAY_REQ");
	}
		break;
	case UT_PETITION_CHAT_USER_END_NFY:
	{
		printf("UT_PETITION_CHAT_USER_END_NFY");
	}
		break;
	case UT_PETITION_CHAT_GM_SAY_RES:
	{
		printf("UT_PETITION_CHAT_GM_SAY_RES");
	}
		break;
	case UT_PETITION_USER_INSERT_REQ:
	{
		printf("UT_PETITION_USER_INSERT_REQ");
	}
		break;
	case UT_PETITION_CONTENT_MODIFY_REQ:
	{
		printf("UT_PETITION_CONTENT_MODIFY_REQ");
	}
		break;
	case UT_PETITION_SATISFACTION_NFY:
	{
		printf("UT_PETITION_SATISFACTION_NFY");
	}
		break;
	case UT_PETITION_USER_CANCEL_REQ:
	{
		printf("UT_PETITION_USER_CANCEL_REQ");
	}
		break;
	case UT_DOJO_BUDOKAI_SEED_ADD_REQ:
	{
		printf("UT_DOJO_BUDOKAI_SEED_ADD_REQ");
	}
		break;
	case UT_DOJO_BUDOKAI_SEED_DEL_REQ:
	{
		printf("UT_DOJO_BUDOKAI_SEED_DEL_REQ");
	}
		break;
	case UT_DOJO_NOTICE_CHANGE_REQ:
	{
		printf("UT_DOJO_NOTICE_CHANGE_REQ");
	}
		break;



	default:
		return CNtlSession::OnDispatch( pPacket );
	}

	return NTL_SUCCESS;
}



//-----------------------------------------------------------------------------------
//		ChatServerMain
//-----------------------------------------------------------------------------------
int ChatServerMain(int argc, _TCHAR* argv[])
{
	CChatServer app;
	CNtlFileStream traceFileStream;

// LOG FILE
	int rc = traceFileStream.Create( "chatlog" );
	if( NTL_SUCCESS != rc )
	{
		printf( "log file CreateFile error %d(%s)", rc, NtlGetErrorMessage( rc ) );
		return rc;
	}
// CHECK INI FILE AND START PROGRAM
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
