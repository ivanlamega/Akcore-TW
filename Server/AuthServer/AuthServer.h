#pragma once

#include "NtlSfx.h"
#include "NtlPacketEncoder_TW.h"
#include "mysqlconn_wrapper.h"
#include "PacketTrace.h"
enum APP_LOG
{
	PRINT_APP = 2,
};
enum AUTH_SESSION
{
	SESSION_CLIENT,
	SESSION_SERVER_ACTIVE,
};
struct sSERVERCONFIG
{
	CNtlString		strClientAcceptAddr;
	WORD			wClientAcceptPort;
	CNtlString		ExternalIP;
	CNtlString		Host;
	CNtlString		User;
	CNtlString		Password;
	CNtlString		Database;
};

class CAuthServer;

//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//

class CClientSession : public CNtlSession
{
public:

	CClientSession(bool bAliveCheck = false, bool bOpcodeCheck = false)
		:CNtlSession(SESSION_CLIENT)
	{
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
	// Packet functions
	void						SendCharLogInReq(CNtlPacket * pPacket, CAuthServer * app);
	void						SendLoginDcReq(CNtlPacket * pPacket);
	// End Packet functions
//private:
	CNtlPacketEncoder_TW	m_packetEncoder;

};

//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------//

class CAuthSessionFactory : public CNtlSessionFactory
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

class CAuthServer : public CNtlServerApp
{
public:
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
	//For Multiple Server - Luiz45
	const string GetConfigFileEnabledMultipleServers()
	{
		return EnableMultipleServers.GetString();
	}
	const DWORD GetConfigFileMaxServers()
	{
		return MAX_NUMOF_SERVER;
	}

	const char*		GetConfigFileExternalIP()
	{
		std::cout << m_config.ExternalIP.c_str() << std::endl;
		return m_config.ExternalIP.c_str();
	}
	int	OnInitApp()
	{
		m_nMaxSessionCount = MAX_NUMOF_SESSION;

		m_pSessionFactory =  new CAuthSessionFactory;
		if( NULL == m_pSessionFactory )
		{
			return NTL_ERR_SYS_MEMORY_ALLOC_FAIL;
		}

		return NTL_SUCCESS;
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
		if( !file.Read("IPAddress", "Address", m_config.ExternalIP) )
		{
			return NTL_ERR_SYS_CONFIG_FILE_READ_FAIL;
		}
		//For multiple servers - Luiz45
		if (!file.Read("ServerOptions", "EnableMultipleServers", EnableMultipleServers))
		{
			return NTL_ERR_SYS_CONFIG_FILE_READ_FAIL;
		}
		if (!file.Read("ServerOptions", "MaxServerAllowed", MAX_NUMOF_SERVER))
		{
			return NTL_ERR_SYS_CONFIG_FILE_READ_FAIL;
		}
		if (EnableMultipleServers.GetString() == "true")
		{
			for (int i = 0; i < MAX_NUMOF_SERVER; i++)
			{
				string strNm = "Char Server" + std::to_string(i);
				if (!file.Read(strNm.c_str(), "Address", ServersConfig[i][0]))
				{
					return NTL_ERR_SYS_CONFIG_FILE_READ_FAIL;
				}
				if (!file.Read(strNm.c_str(), "Port", ServersConfig[i][1]))
				{
					return NTL_ERR_SYS_CONFIG_FILE_READ_FAIL;
				}
			}
		}
 		if( !file.Read("Auth Server", "Address", m_config.strClientAcceptAddr) )
		{
			return NTL_ERR_SYS_CONFIG_FILE_READ_FAIL;
		}
		if( !file.Read("Auth Server", "Port",  m_config.wClientAcceptPort) )
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
		CAuthServer * app = (CAuthServer*)NtlSfxGetApp();
	
		while( IsRunnable() )
		{		
			dwTickCur = ::GetTickCount();
			if( dwTickCur - dwTickOld >= 10000 )
			{

				dwTickOld = dwTickCur;
			}
			Sleep(2);
		}
	}

private:
	CNtlAcceptor				m_clientAcceptor;
	CNtlLog  					m_log;
	sSERVERCONFIG				m_config;
	DWORD						MAX_NUMOF_SERVER = 1;//This will be defined how many servers we can load
	CNtlString					EnableMultipleServers;//Added for enabling multiple server - Luiz45
	DWORD						MAX_NUMOF_GAME_CLIENT = 3;
	DWORD						MAX_NUMOF_SESSION = MAX_NUMOF_GAME_CLIENT + MAX_NUMOF_SERVER;
public:
	MySQLConnWrapper *			db;
	CNtlString					ServersConfig[99][2];//For Config of multiple server 0->{Ip,Port} - Luiz45
	CPacketTrace*				packetTrace;
};