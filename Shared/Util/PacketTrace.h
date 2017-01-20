#include "stdafx.h"
#include <map>
#include "..\..\server\BaseLib\SharedType.h"

class CPacketTrace
{
public:
	CPacketTrace();
	~CPacketTrace();

	

	void	Create();
	void	Destroy();
	void	SetRangeOpCode(int iMin, int iMax = -1);
	
	const char*	GetPacketName(int iOpCode);

	void	Activate();
	void	Deactivate();
	
	//! Event
	

	//! ½Ì±ÛÅæ »ý¼º/Á¦°Å 
	static	void	CreateInstance();
	static	void	DeleteInstance();
	static CPacketTrace*	GetInstance();						///< ½Ì±ÛÅæ ÀÎ½ºÅÏ½º °´Ã¼

private:
	

	typedef std::map< RwUInt32, std::string > PacketEnumNameMap;
	PacketEnumNameMap m_mapPacketName;

	int m_iMinOpCode;
	int m_iMaxOpCode;
	static CPacketTrace*	m_pInstance;
};