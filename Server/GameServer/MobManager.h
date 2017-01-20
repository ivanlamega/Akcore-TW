#pragma once

#include "NtlThread.h"

#include "MonsterClass.h" ///monster class CMonster

#ifndef MOB_MANAGER_H
#define MOB_MANAGER_H   
static RwUInt32 m_uiMobId = 300;


class CMobManager : public CNtlRunObject
{

public:
	CMobManager();
	~CMobManager();

public:
	void					Init();
	void					Release();

	void					CreateMobThread();
	void					Run();

	void					CreateMonsterList();
	void					CreateNpcList();
	void					SpawnNpcAtLogin(CNtlPacket * pPacket, CClientSession * pSession);
	void					SpawnMonsterAtLogin(CNtlPacket * pPacket, CClientSession * pSession);

	RwUInt32				CreateUniqueId(void);
	bool					RunSpawnCheck(CNtlPacket * pPacket, sVECTOR3 curPos, CClientSession * pSession);
	bool					CreatureRangeCheck(sVECTOR3 mycurPos, CNtlVector othercurPos);
	float					Distance(const sVECTOR3 mycurPos, const CNtlVector othercurPos);
	bool					FindCreature(RwUInt32 handle);
	TBLIDX					FindNpc(RwUInt32 handle);
	CMonster::MonsterData*	GetMobByHandle(RwUInt32 m_uiTargetSerialId);
	bool					UpdateDeathStatus(RwUInt32 mobID, bool death_status);
	void					CreateMobByTblidx(RwUInt32 spawnTblidx, RwUInt32 avatarHandle);
	void					CreateNPCByTblidx(RwUInt32 spawnTblidx, RwUInt32 avatarHandle);
private:
	bool m_bRun;
	CNtlThread * pThread;



	std::map<DWORD, CMonster::MonsterData*> m_map_Monster;
	typedef std::map<DWORD, CMonster::MonsterData*>::const_iterator IterType;
	IterType i;

	std::map<DWORD, CMonster::MonsterData*> m_map_Npc;
};
extern CMobManager * g_pMobManager;

#endif //MOB_MANAGER_H