#pragma once

#include "NtlSfx.h"
#include "NtlPacketEncoder_RandKey.h"
#include "mysqlconn_wrapper.h"

#include "NtlFile.h"

#include "NtlPacketUT.h"
#include "NtlPacketTU.h"
#include "ChatFunctions.h"
#include "ResultCode.h"
#include "PacketTrace.h"
#include <iostream>
#include <map>
#include <list>

enum APP_LOG
{
	PRINT_APP = 2,
};
enum CHAT_SESSION
{
	SESSION_CLIENT,
	SESSION_SERVER_ACTIVE,
};
struct sSERVERCONFIG
{
	CNtlString		strClientAcceptAddr;
	WORD			wClientAcceptPort;
	CNtlString		Host;
	CNtlString		User;
	CNtlString		Password;
	CNtlString		Database;
};

const DWORD					MAX_NUMOF_GAME_CLIENT = 3;
const DWORD					MAX_NUMOF_SERVER = 1;
const DWORD					MAX_NUMOF_SESSION = MAX_NUMOF_GAME_CLIENT + MAX_NUMOF_SERVER;
static unsigned int			m_uiSerialId = 0;

class CChatServer;

//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
class CClientSession : public CNtlSession
{
public:
	RwUInt32 AcquireSerialId(void)
	{
		if(m_uiSerialId++)
		{
			if(m_uiSerialId == 0xffffffff)//INVALID_SERIAL_ID)
				m_uiSerialId = 0;
		}
		return m_uiSerialId;
	}
	CClientSession(bool bAliveCheck = false, bool bOpcodeCheck = false)
		:CNtlSession( SESSION_CLIENT )
	{
		SetControlFlag( CONTROL_FLAG_USE_SEND_QUEUE );

		if( bAliveCheck )
		{
			SetControlFlag( CONTROL_FLAG_CHECK_ALIVE );
		}
		if( bOpcodeCheck )
		{
			SetControlFlag( CONTROL_FLAG_CHECK_OPCODE );
		}

		SetPacketEncoder( &m_packetEncoder );
	}

	~CClientSession();

public:
	int							OnAccept();
	void						OnClose();
	int							OnDispatch(CNtlPacket * pPacket);

	unsigned int				GetavatarHandle() { return avatarHandle; }
	int							GetAccountId() { return accountID; }
	int							GetCharacterId() { return characterID; }
	std::string					GetCharName() { return charName; }
	std::string					GetGuildName() { return guildName; }

	// Packet functions
	void						SendEnterChat(CNtlPacket * pPacket, CChatServer * app);

	//Friend and Blacklist functions
	void						SendFriendList(CNtlPacket * pPacket, CChatServer * app);
	void						SendAddFriend(CNtlPacket * pPacket, CChatServer * app);
	void						SendDelFriend(CNtlPacket * pPacket, CChatServer * app);
	void						SendMoveFriend(CNtlPacket * pPacket, CChatServer * app);
	void						SendBlackListAdd(CNtlPacket * pPacket, CChatServer * app);
	void						SendBlackListDel(CNtlPacket * pPacket, CChatServer * app);
	void						SendFriendListMember(CNtlPacket * pPacket, CChatServer * app);
	//Chat Channel Functions
	void						SendSayReq(CNtlPacket * pPacket, CChatServer * app);
	void						SendShoutReq(CNtlPacket * pPacket, CChatServer * app);
	void						SendWhisperReq(CNtlPacket * pPacket, CChatServer * app);
	void						SendFindParty(CNtlPacket * pPacket, CChatServer * app);
	void						SendTradeChatReq(CNtlPacket * pPacket, CChatServer * app);
	void						SendPartyChatReq(CNtlPacket * pPacket, CChatServer * app);
	void						SendMessageTrade(CNtlPacket * pPacket, CChatServer * app);
	void						SendGuildChatReq(CNtlPacket * pPacket, CChatServer * app);
	//Guild Functions
	void						SendLoadGuildInfo(CNtlPacket * pPacket, CChatServer * app);
	void						SendLoadGuildMember(CNtlPacket * pPacket, CChatServer * app, int GuildID);
	void						SendGuildChangeNotice(CNtlPacket * pPacket, CChatServer * app);
	void						SendDisbandGuild(CNtlPacket * pPacket, CChatServer * app);
	void						SendLeaveGuildReq(CNtlPacket * pPacket, CChatServer * app);
	void						SendKickFromGuildReq(CNtlPacket * pPacket, CChatServer * app);
	void						SendNewSecondGuildMaster(CNtlPacket * pPacket, CChatServer * app);
	void						SendRemoveSecondGuildMaster(CNtlPacket * pPacket, CChatServer * app);
	void						SendUpdateGuildMaster(CNtlPacket * pPacket, CChatServer * app);
	void						SendGuildInvitationReq(CNtlPacket * pPacket, CChatServer * app);
	// End Packet functions
private:
	CNtlPacketEncoder_RandKey	m_packetEncoder;

private:
	unsigned int				avatarHandle;
	bool						isHandShaken;
	int							accountID;
	int							characterID;
	std::string					charName;
	std::string					guildName;
};

//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//

class CChatSessionFactory : public CNtlSessionFactory
{
public:

	CNtlSession * CreateSession(SESSIONTYPE sessionType)
	{
		CNtlSession * pSession = NULL;
		switch( sessionType )
		{
		case SESSION_CLIENT: 
			{
				pSession = new CClientSession;
			}
			break;

		default:
			break;
		}

		return pSession;
	}
};

//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//

class CChatServer : public CNtlServerApp
{
public:

	int	OnInitApp()
	{
		m_nMaxSessionCount = MAX_NUMOF_SESSION;

		m_pSessionFactory =  new CChatSessionFactory;
		if( NULL == m_pSessionFactory )
		{
			return NTL_ERR_SYS_MEMORY_ALLOC_FAIL;
		}

		return NTL_SUCCESS;
	}
	const char*		GetConfigFileHost()
	{
		return m_config.Host.c_str();
	}
	const char*		GetConfigFileUser()
	{
		return m_config.User.c_str();
	}
	const char*		GetConfigFilePassword()
	{
		return m_config.Password.c_str();
	}
	const char*		GetConfigFileDatabase()
	{
		return m_config.Database.c_str();
	}
	int	OnCreate()
	{
		int rc = NTL_SUCCESS;
		rc = m_clientAcceptor.Create(	m_config.strClientAcceptAddr.c_str(), m_config.wClientAcceptPort, SESSION_CLIENT, 
										MAX_NUMOF_GAME_CLIENT, 5, 2, MAX_NUMOF_GAME_CLIENT );
		if ( NTL_SUCCESS != rc )
		{
			return rc;
		}
		rc = m_network.Associate( &m_clientAcceptor, true );
		if( NTL_SUCCESS != rc )
		{
			return rc;
		}
		return NTL_SUCCESS;
	}
	void	OnDestroy()
	{
	}
	int	OnCommandArgument(int argc, _TCHAR* argv[])
	{
		return NTL_SUCCESS;
	}
	int	OnConfiguration(const char * lpszConfigFile)
	{
		CNtlIniFile file;
		int rc = file.Create( lpszConfigFile );
		if( NTL_SUCCESS != rc )
		{
			return rc;
		}
		if( !file.Read("Chat Server", "Address", m_config.strClientAcceptAddr) )
		{
			return NTL_ERR_SYS_CONFIG_FILE_READ_FAIL;
		}
		if( !file.Read("Chat Server", "Port",  m_config.wClientAcceptPort) )
		{
			return NTL_ERR_SYS_CONFIG_FILE_READ_FAIL;
		}
		if( !file.Read("DATABASE", "Host",  m_config.Host) )
		{
			return NTL_ERR_DBC_HANDLE_ALREADY_ALLOCATED;
		}
		if( !file.Read("DATABASE", "User",  m_config.User) )
		{
			return NTL_ERR_SYS_MEMORY_ALLOC_FAIL;
		}
		if( !file.Read("DATABASE", "Password",  m_config.Password) )
		{
			return NTL_ERR_SYS_LOG_SYSTEM_INITIALIZE_FAIL;
		}
		if( !file.Read("DATABASE", "Db",  m_config.Database) )
		{
			return NTL_ERR_DBC_CONNECTION_CONNECT_FAIL;
		}
		return NTL_SUCCESS;
	}
	int	OnAppStart()
	{
		return NTL_SUCCESS;
	}
	void	Run()
	{
		DWORD dwTickCur, dwTickOld = ::GetTickCount();

		while( IsRunnable() )
		{		
			dwTickCur = ::GetTickCount();
			if( dwTickCur - dwTickOld >= 10000 )
			{
			//	NTL_PRINT(PRINT_APP, "Chat Server Run()");
				dwTickOld = dwTickCur;
			}
			Sleep(2);
		}
	}

private:
	CNtlAcceptor				m_clientAcceptor;
	CNtlLog  					m_log;
	sSERVERCONFIG				m_config;
public:
	MySQLConnWrapper *			db;
	MySQLConnWrapper *			db2;
	ChatFunctionsClass *		csf;
public:
	bool						AddUser(const char * lpszUserID, CClientSession * pSession)
	{
		if( false == m_userList.insert( USERVAL(CNtlString(lpszUserID), pSession)).second )
		{
			return false;
		}
		return true;		
	}
	void						RemoveUser(const char * lpszUserID)
	{
		m_userList.erase( CNtlString(lpszUserID) );
	}
	bool						FindUser(const char * lpszUserID)
	{
		USERIT it = m_userList.find( CNtlString(lpszUserID) );
		if( it == m_userList.end() )
			return false;

		return true;
	}
	CClientSession*						FindUserSession(const char * lpszUserID)
	{
		USERIT it = m_userList.find(CNtlString(lpszUserID));
		if (it == m_userList.end())
			return NULL;

		return it->second;
	}
	void						UserBroadcast(CNtlPacket * pPacket)
	{
		for( USERIT it = m_userList.begin(); it != m_userList.end(); it++ )
		{
			it->second->PushPacket( pPacket );
		}
	}
	void						UserBroadcastothers(CNtlPacket * pPacket, CClientSession * pSession)
	{
		for( USERIT it = m_userList.begin(); it != m_userList.end(); it++ )
		{
			if(pSession->GetavatarHandle() != it->second->GetavatarHandle())
			it->second->PushPacket( pPacket );
		}
	}
	typedef std::map<CNtlString, CClientSession*> USERLIST;
	typedef USERLIST::value_type USERVAL;
	typedef USERLIST::iterator USERIT;
	USERLIST					m_userList;
};