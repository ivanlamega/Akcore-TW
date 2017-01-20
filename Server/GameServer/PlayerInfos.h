#include "stdafx.h"
#include "Vector.h"
#include <list>
#include <cstdio>
#include <cstdlib>
#include "Avatar.h"
#include "Character.h"
#include <iostream>

class CClientSession;

class PlayerInfos
{
public:
	PlayerInfos()
	{
		this->pcProfile = new sPC_PROFILE;
		this->sCharState = new sCHARSTATE;
		this->CurRPBall = 0;
		LastPartyHandle = -1;
		dwThreadId = 0;
		ChargingID = 1;
		CurRPBallOk = 0;
		isKaioken = false;
		isSitted = false;
		isSsj = false;
		isMajin = false;
		isGreatNamek = false;
		isfighting= false;
		lastFightTime = 0;
		isdead = false;
	};
	~PlayerInfos()
	{
		if (TerminateThread(hThread, 1) == 0)
			printf("Can't kill thread regen\n");
		if (hThread)
			CloseHandle(hThread);
	};
	sPC_PROFILE		*pcProfile;
	sCHARSTATE		*sCharState;
	HSESSION		MySession;
	void		UpdateLP();
	void		UpdateEP();
	void		setPlayerStat(sPC_PROFILE* pc, sCHARSTATE *sCharSt)
	{	
		memcpy(this->pcProfile, pc, sizeof(sPC_PROFILE));
		memcpy(this->sCharState, sCharSt, sizeof(sCHARSTATE));
	};
	void		StoreSession(HSESSION ss){this->MySession = ss;};
	void		ClearTheList();
	sVECTOR3	GetPosition(){return this->vCurLoc;};
	sVECTOR3	GetDirection(){return this->vCurDir;};
	void		SetPosition(const sVECTOR3 curPos, const sVECTOR3 curDir)
	{
		this->vCurLoc.x = curPos.x;
		this->vCurLoc.y = curPos.y;
		this->vCurLoc.z = curPos.z;

		this->vCurDir.x = curDir.x;
		this->vCurDir.y = curDir.y;
		this->vCurDir.z = curDir.z;
	};
	int			GetAccountID(){return this->AccountID;};
	void		SetAccountID(const int id){this->AccountID = id;};
	void		SetGuildName(const std::string name){this->guildname = name;};
	std::string		GetGuildName(){return this->guildname;};
	void		SetPlayerName(const std::string name){this->username = name;};
	std::string		GetPlayerName(){return this->username;};
	void		SetWorldID(const int id){this->WorldID = id;};
	int			GetWorldID(){return this->WorldID;};
	void		SetWorldTableID(const int id){this->WorldTableID = id;};
	int			GetWorldTableID(){return this->WorldTableID;};
	void		Setmob_SpawnTime(const RwUInt32 id){this->mob_SpawnTime = id;};
	RwUInt32	Getmob_SpawnTime(){return this->mob_SpawnTime;};
	void		setMyAPP(CGameServer * _app){this->app = _app;};
	void		Setlast_SpawnPos(const sVECTOR3 id){this->last_SpawnPos = id;};
	sVECTOR3	Getlast_SpawnPos(){return this->last_SpawnPos;};
	// PLAYER STAT CALCULE
	void		calculeMyStat(CGameServer * app);
	void		setZero();
	void		UpdateAttribute(RwUInt32 Handle, RwUInt32 Attribute, RwUInt32 Amount);

	void		StoreHandle(const RwUInt32 _avatarHandle){this->avatarHandle = _avatarHandle;};
	RwUInt32	GetAvatarandle(){return this->avatarHandle;};
	int			getNumberOfRPBall(){return this->CurRPBall;};
	void		UpdateRPBall()
	{
		CNtlPacket packet5(sizeof(sGU_UPDATE_CHAR_RP_BALL_MAX));	
 		sGU_UPDATE_CHAR_RP_BALL_MAX * maxBall = (sGU_UPDATE_CHAR_RP_BALL_MAX*)packet5.GetPacketData();
 		CNtlPacket packet6(sizeof(sGU_UPDATE_CHAR_RP_BALL));	
 		sGU_UPDATE_CHAR_RP_BALL * ball = (sGU_UPDATE_CHAR_RP_BALL*)packet6.GetPacketData();
 
 		maxBall->byMaxRPBall = this->getNumberOfRPBall();
 		maxBall->handle = this->GetAvatarandle();
 		maxBall->wOpCode = GU_UPDATE_CHAR_RP_BALL_MAX;
 
 		ball->bDropByTime = true;
 		ball->byCurRPBall = this->getNumberOfRPBall();
 		ball->handle = this->GetAvatarandle();
 		ball->wOpCode = GU_UPDATE_CHAR_RP_BALL;
 
 		packet5.SetPacketLen(sizeof(sGU_UPDATE_CHAR_RP_BALL_MAX));
 		g_pApp->Send(this->MySession, &packet5);
 
 		packet6.SetPacketLen(sizeof(sGU_UPDATE_CHAR_RP_BALL));
 		g_pApp->Send(this->avatarHandle, &packet6);
	};
	void		SetStartRPBall()
	{
		if (this->pcProfile->byLevel >= 5 && this->pcProfile->byLevel <= 9)
			this->CurRPBall = 1;
		else if (this->pcProfile->byLevel >= 15 && this->pcProfile->byLevel <= 24)
			this->CurRPBall = 2;
		else if (this->pcProfile->byLevel >= 25 && this->pcProfile->byLevel <= 29)
			this->CurRPBall = 3;
		else if (this->pcProfile->byLevel >= 30 && this->pcProfile->byLevel <= 34)
			this->CurRPBall = 4;
		else if (this->pcProfile->byLevel >= 35 && this->pcProfile->byLevel <= 39)
			this->CurRPBall = 5;
		else if (this->pcProfile->byLevel >= 40 && this->pcProfile->byLevel <= 44)
			this->CurRPBall = 6;
		else if (this->pcProfile->byLevel >= 45)
			this->CurRPBall = 7;
		else if (this->pcProfile->byLevel < 5)
			this->CurRPBall = 0;
		if (this->pcProfile->byLevel >= 5)
			this->UpdateRPBall();
	};
	void		SetLevelup(sPC_TBLDAT *Data)
	{
		this->byLevel_Up_Energy_Defence = Data->byLevel_Up_Energy_Defence;
		this->byLevel_Up_Energy_Offence = Data->byLevel_Up_Energy_Offence;
		this->byLevel_Up_EP = Data->byLevel_Up_EP;
		this->byLevel_Up_LP = Data->byLevel_Up_LP;
		this->byLevel_Up_Physical_Defence = Data->byLevel_Up_Physical_Defence;
		this->byLevel_Up_Physical_Offence = Data->byLevel_Up_Physical_Offence;
		this->byLevel_Up_RP = Data->byLevel_Up_RP;
		this->fLevel_Up_Con = Data->fLevel_Up_Con;
		this->fLevel_Up_Dex = Data->fLevel_Up_Dex;
		this->fLevel_Up_Eng = Data->fLevel_Up_Eng;
		this->fLevel_Up_Foc = Data->fLevel_Up_Foc;
		this->fLevel_Up_Sol = Data->fLevel_Up_Sol;
		this->fLevel_Up_Str = Data->fLevel_Up_Str;

		/* TEMPORARY */
		this->pcProfile->avatarAttribute.wBaseLpRegen = 15;
		this->pcProfile->avatarAttribute.wBaseEpRegen = 15;
		this->pcProfile->avatarAttribute.wLastEpRegen = 15;
		this->pcProfile->avatarAttribute.wLastLpRegen = 15;
		/* TEMPORARY */
	};
	void		LevelUpPlayer()
	{
		this->pcProfile->avatarAttribute.wBasePhysicalOffence += (WORD)this->byLevel_Up_Physical_Offence;
		this->pcProfile->avatarAttribute.wBasePhysicalDefence += (WORD)this->byLevel_Up_Physical_Defence;
		this->pcProfile->avatarAttribute.wBaseEnergyOffence += (WORD)this->byLevel_Up_Energy_Offence;
		this->pcProfile->avatarAttribute.wBaseEnergyDefence += (WORD)this->byLevel_Up_Energy_Defence;
		this->pcProfile->avatarAttribute.byBaseCon += (BYTE)this->fLevel_Up_Con;
		this->pcProfile->avatarAttribute.byBaseDex += (BYTE)this->fLevel_Up_Dex;
		this->pcProfile->avatarAttribute.byBaseEng += (BYTE)this->fLevel_Up_Eng;
		this->pcProfile->avatarAttribute.byBaseFoc += (BYTE)this->fLevel_Up_Foc;
		this->pcProfile->avatarAttribute.byBaseSol += (BYTE)this->fLevel_Up_Sol;
		this->pcProfile->avatarAttribute.byBaseStr += (BYTE)this->fLevel_Up_Str;
		this->pcProfile->avatarAttribute.wBaseMaxRP += (WORD)this->byLevel_Up_RP;
		this->pcProfile->avatarAttribute.wBaseMaxEP = (WORD)(this->pcProfile->avatarAttribute.wBaseMaxEP + this->byLevel_Up_EP);
		this->pcProfile->avatarAttribute.wBaseMaxEP = this->pcProfile->avatarAttribute.wBaseMaxEP + (WORD)(this->pcProfile->avatarAttribute.byLastEng * 4.2);
		this->pcProfile->avatarAttribute.wBaseMaxLP = (this->pcProfile->avatarAttribute.wBaseMaxLP + (WORD)this->byLevel_Up_LP);
		this->pcProfile->avatarAttribute.wBaseMaxLP = this->pcProfile->avatarAttribute.wBaseMaxLP + (WORD)(this->pcProfile->avatarAttribute.byLastCon* 4.7);
		if (this->pcProfile->byLevel == 5)
			this->CurRPBall = 1;
		if (this->pcProfile->byLevel == 15)
			this->CurRPBall = 2;
		if (this->pcProfile->byLevel == 25)
			this->CurRPBall = 3;
		if (this->pcProfile->byLevel == 30)
			this->CurRPBall = 4;
		if (this->pcProfile->byLevel == 35)
			this->CurRPBall = 5;
		if (this->pcProfile->byLevel == 40)
			this->CurRPBall = 6;
		if (this->pcProfile->byLevel == 45)
			this->CurRPBall = 7;
		else if (this->pcProfile->byLevel < 5)
			this->CurRPBall = 0;

		CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_LP_EP));
		sGU_UPDATE_CHAR_LP_EP * res = (sGU_UPDATE_CHAR_LP_EP *)packet.GetPacketData();
		this->pcProfile->wCurEP = this->pcProfile->avatarAttribute.wBaseMaxEP;
		this->pcProfile->wCurLP = this->pcProfile->avatarAttribute.wBaseMaxLP;

		res->handle = this->avatarHandle;
		res->wCurEP = this->pcProfile->wCurEP;
		res->wCurLP = this->pcProfile->wCurLP;
		res->wMaxEP = this->pcProfile->wCurEP;
		res->wMaxLP = this->pcProfile->wCurLP;
		res->wOpCode = GU_UPDATE_CHAR_LP_EP;

		packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LP_EP));
		g_pApp->Send(this->MySession, &packet);
	}
	void		LastPartyInvited(){};
	void		SpawnMyChar();
	void		setRpBallOk(int rp){CurRPBallOk = rp;};
	int			getRpBallOk(){return CurRPBallOk;};
	void		UpdateRpBallOk(int moreOrLess /* 0+ 1- */)
	{
		CNtlPacket packet6(sizeof(sGU_UPDATE_CHAR_RP_BALL));	
	 	sGU_UPDATE_CHAR_RP_BALL * ball = (sGU_UPDATE_CHAR_RP_BALL*)packet6.GetPacketData();

		if (moreOrLess == 0)
		{
			if (this->getRpBallOk() <= this->getNumberOfRPBall()) // i'm tired to change this error so please update this fucking file in your local depo :O
			{
				this->setRpBallOk(this->getRpBallOk() + 1);
				ball->bDropByTime = true;
			}
		}
		else
		{
			if (this->getRpBallOk() >= 1)
			{
				this->setRpBallOk(this->getRpBallOk() - 1);
				ball->bDropByTime = false;
			}
		}
		ball->byCurRPBall = this->getRpBallOk();
		ball->handle = this->GetAvatarandle();
		ball->wOpCode = GU_UPDATE_CHAR_RP_BALL;
		packet6.SetPacketLen(sizeof(sGU_UPDATE_CHAR_RP_BALL));
	 	g_pApp->Send(this->MySession, &packet6);
	};
	void		SendPlayerLifeAndEP();
	void		checkBuff(int skill);
	void		TransformCancel()
	{
		this->isKaioken = false;
		this->isSsj = false;
		this->isMajin = false;
		this->isGreatNamek = false;

		this->sCharState->sCharStateBase.aspectState.sAspectStateDetail.sKaioken.byRepeatingCount = 0;
	};
	void		UpdateRP();
	void		TakeDamage(int Damage);
	void		setFightMod(bool fight){this->isfighting = fight;};
	bool		getFightMod(){return this->isfighting;};
	void		setDeadMod(){this->isdead = true;};
	bool		getDeadMod(){return this->isdead;};
	void		UpdateDeathStatus();
	void		UpdateRevivalStatus();
	PlayerInfos	*refreshPointer(){return this;};
private:
	MySQLConnWrapper			*db;
public:
DWORD		GetMoney();
int			GetBankMoney();
void		SetMoney(int Amount);
void		SetBankMoney(int Amount);
void		SaveMe();
void		SavePlayerData();
HANDLE		Charging_Thread;
HANDLE      hThread;
DWORD		dwThreadId;
DWORD		ChargingID;
bool		isKaioken;
bool		isSsj;
bool		isMajin;
bool		isSitted;
bool		isGreatNamek;
DWORD		lastFightTime;
CClientSession *myCCSession;
private:
	sVECTOR3			vCurLoc;
	sVECTOR3			vCurDir;
	int					AccountID;
	int					WorldID;
	int					WorldTableID;
	std::string			guildname;
	std::string			username;
	RwUInt32			mob_SpawnTime;
	sVECTOR3			last_SpawnPos;
	RwUInt32			avatarHandle;
	CGameServer *		app;
	int					CurRPBall;
	int					CurRPBallOk;
	bool				isfighting;
	bool				isdead;
public: // THIS NEED BE BE PRIVATE IN THE FUTUR
	BYTE			byLevel_Up_LP;
	BYTE			byLevel_Up_EP;
	BYTE			byLevel_Up_RP;
	BYTE			byLevel_Up_Physical_Offence;
	BYTE			byLevel_Up_Physical_Defence;
	BYTE			byLevel_Up_Energy_Offence;
	BYTE			byLevel_Up_Energy_Defence;

	float			fLevel_Up_Str;
	float			fLevel_Up_Con;
	float			fLevel_Up_Foc;
	float			fLevel_Up_Dex;
	float			fLevel_Up_Sol;
	float			fLevel_Up_Eng;
	int				LastPartyHandle;
};