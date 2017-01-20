#pragma once


#include "NtlConnection.h"
#include <map>
// Session delete macro
#define RELEASE_SESSION(p)			if(p) { p->Release(); p = NULL; }


class CNtlSession : public CNtlConnection
{
	friend class CNtlSessionList;

public:

	CNtlSession(SESSIONTYPE sessionType);
	virtual ~CNtlSession();


public:

	virtual int						OnConnect() { return NTL_SUCCESS; }

	virtual int						OnAccept() { return NTL_SUCCESS; }

	virtual void					OnClose() {}

	virtual int						OnDispatch(CNtlPacket * pPacket);

	//int								Send(CNtlPacket * pPacket) { return CNtlConnection::PushPacket( pPacket ); }


public:

	HSESSION						GetHandle() { return m_hSession; }

	SESSIONTYPE						GetSessionType() { return m_sessionType; }


public:

	void							Acquire();

	void							Release();


public:

	static bool						IsInternalConnection(char* pIp);
	const char*						GetPacketName(int iOpCode);
	void							SetRangeOpCode(int iMin, int iMax);
	
	
	typedef std::map< unsigned int, std::string > PacketEnumNameMap;
	PacketEnumNameMap m_mapPacketName;

	int m_iMinOpCode;
	int m_iMaxOpCode;


protected:

	void							Init();

	void							Destroy();


private:

	CNtlSession&					operator=(CNtlSession&);


private:

	DWORD							m_dwReferenceCount;

	HSESSION						m_hSession;

	const SESSIONTYPE				m_sessionType;



};

class CNtlSessionAutoPtr
{

public:
	explicit CNtlSessionAutoPtr( CNtlSession * pSession ) 
		: m_pSession ( pSession ) 
	{
	}

	~CNtlSessionAutoPtr( ) 
	{ 
		RELEASE_SESSION( m_pSession );
	}

private:
	CNtlSession * m_pSession;

};//end of class ( CNtlSessionAutoPtr )



