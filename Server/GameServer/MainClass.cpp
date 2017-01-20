#include "stdafx.h"
#include "Attributes.h"
#include "Inventory.h"
#include "SkillsQS.h"
#include "MainClass.h"
#include "GameServer.h"
RwUInt8 PlayersMain::byChainAttack = 0;
PlayersMain::PlayersMain(HSESSION PlayerSession,int CharID,int AccountID)
{
	this->PlayerSession = PlayerSession;
	this->AccountID = AccountID;
	this->sPlayerProfile = new sPC_PROFILE;
	this->sPlayerProfile->charId = CharID;
	this->sPlayerState = new sCHARSTATE;
	this->cPlayerInventory = new PlayersInventory(this->GetCharID());
	this->cPlayerSkills = new PlayersSkills(this->GetCharID());
	this->cPlayerAttribute = new PlayerAttributes(this->PlayerSession);
	this->isDead = false;
	this->isSitted = false;
	this->isfighting = false;
	this->bTradeOk = false;
	this->bPlayerInTrade = false;
	this->isInTutorial = false;
	this->bDuelStatus = false;
	this->bSkillInUse = false;
	this->PlayerThreadID = 0;
	this->RpBallFull = 0;	
	this->ChargingID = 1;
	this->lastFightTime = 0;
	this->CharIDForDuel = 0;
	this->byChainAttack = 0;
	for (int i = 0; i < 8; i++){
		this->tblEquipedChips[i] = INVALID_TBLIDX;
	}
}
PlayersMain::~PlayersMain()
{
}
//Returns the sPC_PROFILE
sPC_PROFILE* PlayersMain::GetPcProfile()
{
	return this->sPlayerProfile;
}
//Returns Player Sit
bool PlayersMain::GetPlayerSitGetUp()
{
	return this->isSitted;
}
//Just return if the player have any space in his scouter TODO: See what the value for each scout slot
bool PlayersMain::HaveAnySpaceInScouter()
{
	for (int i = 0; i < 4; i++){
		if (this->tblEquipedChips[i] == INVALID_TBLIDX)
			return true;
		else
			continue;
	}
	return false;
}
//Returns The player is in Tutorial?
bool PlayersMain::IsInTutorial()
{
	return this->isInTutorial;
}
//Returns Player Fight?
bool PlayersMain::GetPlayerFight()
{
	return this->isfighting;
}
//Return if Player trade is ok
bool PlayersMain::GetTradeOK()
{
	return this->bTradeOk;
}
//Returns Player Dead?
bool PlayersMain::GetPlayerDead()
{
	return this->isDead;
}
//Returns if the player is using skill
bool PlayersMain::GetSkillInUse()
{
	return this->bSkillInUse;
}
//Returns Player is In Trade?
bool PlayersMain::GetPlayerIsInTrade()
{
	return this->bPlayerInTrade;
}
//Return Player is in Duel
bool PlayersMain::GetDuelStatus()
{
	return this->bDuelStatus;
}
//Returns the sCHARSTATE
sCHARSTATE* PlayersMain::GetCharState()
{
	return this->sPlayerState;
}
//Returns the Char Handle
RwUInt32 PlayersMain::GetAvatarHandle()
{
	//We dont know why but it solves the problem :) - Thanks ebbo
	//try{
		return this->avatarHandle;
	//}
	//catch (exception e)
	//{
	//	printf("Handle is invalid\n\r %s", e.what());
	//}
}
//Returns Mob Spawn Time
RwUInt32 PlayersMain::GetMob_SpawnTime()
{
	return this->mob_SpawnTime;
}
//Returns Charging Thread ID
DWORD PlayersMain::GetChargingID()
{
	return this->ChargingID;
}
//Returns Chargin Thread
HANDLE PlayersMain::GetChargingThread()
{
	return this->ChargingThread;
}
//Returns the PlayerSession session
HSESSION PlayersMain::GetSession()
{
	return this->PlayerSession;
}
//Returns the AccountID
int PlayersMain::GetAccountID()
{
	return this->AccountID;
}
//Returns the CharID
int PlayersMain::GetCharID()
{
	return this->sPlayerProfile->charId;
}
//Return how many RP Balls this player Have
int PlayersMain::GetRpBallCounter()
{
	return this->RPCounter;
}
//Returns how many RP Ball are full
int PlayersMain::GetRpBallFull()
{
	return this->RpBallFull;
}
//Returns Player Position
sVECTOR3 PlayersMain::GetPlayerPosition()
{
	return this->vCurLoc;
}
//Returns Player Last Position - This will be used for Respawn at Popo Stone
sVECTOR3 PlayersMain::GetPlayerLastPosition()
{
	return this->vLastLoc;
}
//Returns Player Direction
sVECTOR3 PlayersMain::GetPlayerDirection()
{
	return this->vCurDir;
}
//Returns Player Last Direction - This will be used for Respawn at Popo Stone
sVECTOR3 PlayersMain::GetPlayerLastDirection()
{
	return this->vLastDir;
}
//Returns Actual World ID
int PlayersMain::GetWorldID()
{
	return this->iWorldID;
}
//Returns the last time figh
DWORD PlayersMain::GetLastFightTime()
{
	return this->lastFightTime;
}
//Returns Player Thread ID
DWORD PlayersMain::GetPlayerThreadID()
{
	return this->PlayerThreadID;
}
//Returns Player Handle for Duel
int PlayersMain::GetCharIdForDuel()
{
	return this->CharIDForDuel;
}
//Returns Player Thread
HANDLE PlayersMain::GetPlayerThread()
{
	return this->PlayerThread;
}
//Return an Array of Equiped Chips
TBLIDX* PlayersMain::GetEquipedChips()
{
	return this->tblEquipedChips;
}
//Returns Actual World Tblidx
TBLIDX PlayersMain::GetWorldTblx()
{
	return this->WorldTblx;
}
//Returns Player Name
string PlayersMain::GetPlayerName()
{
	return this->sPlayerName;
}
//Returns Guild Name
string PlayersMain::GetGuildName()
{
	return this->sGuildName;
}
//Returns Chain attack number
RwUInt8 PlayersMain::GetChainAttack()
{
	return this->byChainAttack;
}
//Sets ChainAttack
void PlayersMain::SetChainAttack(int index)
{
	this->byChainAttack = index;
}
//Sets our Chargin RP Thread ID
void PlayersMain::SetChargingID(DWORD id)
{
	this->ChargingID = id;
}
//Sets our Handle
void PlayersMain::SetAvatarHandle(RwUInt32 AvatarHandle)
{
	this->avatarHandle = AvatarHandle;
}
//Set Player Sit or Get Up
void PlayersMain::SetPlayerSit(bool SiteGetUp)
{
	this->isSitted = SiteGetUp;
}
//Sets Player Dead or Alive
void PlayersMain::SetPlayerDead(bool isDead)
{
	this->isDead = isDead;
}
//Sets Player Trade OK
void PlayersMain::SetTradeOK(bool bTradeOK)
{
	this->bTradeOk = bTradeOK;
}
//Sets the Player is In trade
void PlayersMain::SetPlayerIsInTrade(bool bIsInTrade)
{
	this->bPlayerInTrade = bIsInTrade;
}
//Sets the Player is in duel
void PlayersMain::SetDuelStatus(bool bStatus)
{
	this->bDuelStatus = bStatus;
}
//Sets our Spawn Time
void PlayersMain::SetMob_SpawnTime(RwUInt32 id)
{
	this->mob_SpawnTime = id;
}
//Sets our Charging Thread
void PlayersMain::SetChargindThread(HANDLE thread)
{
	this->ChargingThread = thread;
}
//Sets We are using Skill
void PlayersMain::SetSkillInUse(bool inUse)
{
	this->bSkillInUse = inUse;
}
//Sets How many Rp Ball are full
void PlayersMain::SetRpBallFull(int moreOrLess)
{
	this->RpBallFull = moreOrLess;
}
//Sets Our Charstate by ID
void PlayersMain::SetCharState(int charState)
{
	this->sPlayerState->sCharStateBase.byStateID = charState;
}
//This will fill our sPC_Profile with Attributes
void PlayersMain::FillProfileWithAttribute()
{
	this->cPlayerAttribute->LoadAttributesFromDB(this->GetCharID());
	this->sPlayerProfile->avatarAttribute = this->cPlayerAttribute->GetAvatarAttribute();
}
//This will fill our sPC_Profile with the Respective CharState
void PlayersMain::FillCharState()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	MySQLConnWrapper *db = new MySQLConnWrapper;
	db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
	db->connect();
	db->switchDb(app->GetConfigFileDatabase());

	//Makesure this sets everything to 0 before assigning things
	//memset(this->sPlayerState, 0, sizeof(this->sPlayerState));

	db->prepare("SELECT * FROM characters WHERE CharID = ?");
	db->setInt(1, this->GetCharID());
	db->execute();
	db->fetch();
	this->sPlayerState->sCharStateBase.vCurLoc.x = (float)db->getDouble("CurLocX");
	this->sPlayerState->sCharStateBase.vCurLoc.y = (float)db->getDouble("CurLocY");
	this->sPlayerState->sCharStateBase.vCurLoc.z = (float)db->getDouble("CurLocZ");
	this->sPlayerState->sCharStateBase.vCurDir.x = (float)db->getDouble("CurDirX");
	this->sPlayerState->sCharStateBase.vCurDir.y = (float)db->getDouble("CurDirY");
	this->sPlayerState->sCharStateBase.vCurDir.z = (float)db->getDouble("CurDirZ");
	this->sPlayerState->sCharStateBase.dwConditionFlag = 0xff;
	this->sPlayerState->sCharStateBase.byStateID = CHARSTATE_STANDING;
	this->sPlayerState->sCharStateBase.bFightMode = false;
	this->sPlayerState->sCharStateBase.dwStateTime = 0;
	this->sPlayerState->sCharStateBase.unknow1 = 0;
	/*for (int i = 0; i < 11; i++)
	this->sPlayerState->sCharStateBase.aspectState.sAspectStateDetail.unknown[i] = 0;*/

	
	this->sPlayerState->sCharStateBase.aspectState.sAspectStateBase.byAspectStateId = 0xff;
	this->sPlayerState->sCharStateBase.aspectState.sAspectStateDetail.sGreatNamek.bySourceGrade = 0xff;
	this->sPlayerState->sCharStateBase.aspectState.sAspectStateDetail.sKaioken.bySourceGrade = 0xff;
	this->sPlayerState->sCharStateBase.aspectState.sAspectStateDetail.sPureMajin.bySourceGrade = 0xff;
	this->sPlayerState->sCharStateBase.aspectState.sAspectStateDetail.sSuperSaiyan.bySourceGrade = 0xff;
	this->sPlayerState->sCharStateBase.aspectState.sAspectStateDetail.sVehicle.idVehicleTblidx = 0xff;
}
//This will fill our sPC_Profile with Other Infos(PC Shape,Level,Name,blabla)
void PlayersMain::FillProfileWithInfo()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	MySQLConnWrapper *db = new MySQLConnWrapper;
	db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
	db->connect();
	db->switchDb(app->GetConfigFileDatabase());
	
	db->prepare("SELECT * FROM characters WHERE CharID = ?");
	db->setInt(1, this->GetCharID());
	db->execute();
	db->fetch();
	
	//Getting right Table
	/*CPCTable *pPcTable = app->g_pTableContainer->GetPcTable();
	sPC_TBLDAT *pTblData = (sPC_TBLDAT*)pPcTable->GetPcTbldat(db->getInt("Race"), db->getInt("Class"), db->getInt("Gender"));*/
	dbo_data_table_pc *pc = new dbo_data_table_pc();
	pc->load("data/table_pc_data.rdf");
	const dbo_data_table_pc_st *pcDat = pc->pc_data_get(db->getInt("Race"), db->getInt("Class"), db->getInt("Gender"));
	this->sPlayerProfile->byMaxLevel = 70;
	//this->sPlayerProfile->tblidx = pTblData->tblidx;
	this->sPlayerProfile->tblidx = pcDat->id;
	this->sPlayerProfile->bChangeClass = db->getBoolean("ChangeClass");
	this->sPlayerProfile->bIsAdult = db->getBoolean("Adult");
	this->sPlayerProfile->charId = db->getInt("CharID");
	wcscpy_s(this->sPlayerProfile->awchName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(db->getString("CharName")).c_str());
	//PC Shape
	this->sPlayerProfile->sPcShape.byFace = db->getInt("Face");
	this->sPlayerProfile->sPcShape.byHair = db->getInt("Hair");
	this->sPlayerProfile->sPcShape.byHairColor = db->getInt("HairColor");
	this->sPlayerProfile->sPcShape.bySkinColor = db->getInt("SkinColor");
	//Other Infos
	this->sPlayerProfile->byLevel = db->getInt("Level");
	this->sPlayerProfile->dwCurExp = db->getInt("Exp");
	this->sPlayerProfile->dwMaxExpInThisLevel = db->getInt("MaxExpInThisLevel");
	this->sPlayerProfile->dwZenny = db->getInt("Money");
	this->sPlayerProfile->dwTutorialHint = -1;
	this->sPlayerProfile->byBindType = 0;
	this->sPlayerProfile->bindObjectTblidx = -1;

	this->sPlayerProfile->dwReputation = db->getInt("Reputation");
	this->sPlayerProfile->dwMudosaPoint = db->getInt("MudosaPoint");
	this->sPlayerProfile->dwSpPoint = db->getInt("SpPoint");
	this->sPlayerProfile->bIsGameMaster = db->getBoolean("GameMaster");
	this->sPlayerProfile->sMarking.dwCode = db->getInt("titulo");
	this->sPlayerProfile->sMixData.bNormalStart = 0;
	this->sPlayerProfile->sMixData.bSpecialStart = 0;
	this->sPlayerProfile->sMixData.byMixLevel = db->getInt("MixLevel");
	this->sPlayerProfile->sMixData.bySpecialType = 0;
	this->sPlayerProfile->sMixData.dwMixExp = db->getInt("MixExp");
	this->sPlayerProfile->sMixData.unknown = 0;
	this->sPlayerProfile->dwCurLP = db->getInt("CurLP");
	this->sPlayerProfile->wCurEP = db->getInt("CurEP");
	this->sPlayerProfile->wCurRP = db->getInt("CurRP");
	this->sPlayerProfile->dwCurAp = db->getInt("CurAp");//New AP TW
	//this->sPlayerProfile->netP = 0;
	//this->sPlayerProfile->unknown2 = 0;// Unknow TW
	this->sPlayerProfile->sLocalize.type = 0x03;//Unknow Thing maybe is the same as Third for Taiwan Localize
	memcpy(this->sPlayerProfile->sLocalize.unknown, "\x24\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\x00\x50\x00\x00\x00\x00\xff\xff\xff\xff\xff\x00\x00\x00\x00\x00", 27);//Some expert need see what is this?

	sVECTOR3 vLastDir;
	sVECTOR3 vLastLoc;

	vLastLoc.x = (float)db->getDouble("CurLocX");
	vLastLoc.y = (float)db->getDouble("CurLocY");
	vLastLoc.z = (float)db->getDouble("CurLocZ");

	vLastDir.x = (float)db->getDouble("CurDirX");
	vLastDir.y = (float)db->getDouble("CurDirY");
	vLastDir.z = (float)db->getDouble("CurDirZ");

	//this->sPcData = pTblData;
	this->SetPlayerName(db->getString("CharName"));
	this->SetGuildName(db->getString("GuildName"));
	this->SetWorldID(db->getInt("WorldID"));
	this->SetWorldTblidx(db->getInt("WorldTable"));
	this->SetPlayerPosition(vLastLoc);
	this->SetPlayerLastDirection(vLastDir);
}
//Sets How many Rage Power Ball's the player gonna have it
void PlayersMain::SetRPBall()
{
	if (this->sPlayerProfile->byLevel >= 5 && this->sPlayerProfile->byLevel <= 9)
		this->RPCounter = 1;
	else if (this->sPlayerProfile->byLevel >= 15 && this->sPlayerProfile->byLevel <= 24)
		this->RPCounter = 2;
	else if (this->sPlayerProfile->byLevel >= 25 && this->sPlayerProfile->byLevel <= 29)
		this->RPCounter = 3;
	else if (this->sPlayerProfile->byLevel >= 30 && this->sPlayerProfile->byLevel <= 34)
		this->RPCounter = 4;
	else if (this->sPlayerProfile->byLevel >= 35 && this->sPlayerProfile->byLevel <= 39)
		this->RPCounter = 5;
	else if (this->sPlayerProfile->byLevel >= 40 && this->sPlayerProfile->byLevel <= 44)
		this->RPCounter = 6;
	else if (this->sPlayerProfile->byLevel >= 45)
		this->RPCounter = 7;
	else if (this->sPlayerProfile->byLevel < 5)
		this->RPCounter = 0;
}
//Sets Player Position
void PlayersMain::SetPlayerPosition(sVECTOR3 sPlayerPosition)
{
	this->vCurLoc.x = sPlayerPosition.x;
	this->vCurLoc.y = sPlayerPosition.y;
	this->vCurLoc.z = sPlayerPosition.z;
}
//Sets Player Last Position - This Will be used for Popo Stone
void PlayersMain::SetPlayerLastPosition(sVECTOR3 sPlayerPosition)
{
	this->vLastLoc.x = sPlayerPosition.x;
	this->vLastLoc.y = sPlayerPosition.y;
	this->vLastLoc.z = sPlayerPosition.z;
}
//Sets Player Direction
void PlayersMain::SetPlayerDirection(sVECTOR3 sPlayerDirection)
{
	this->vCurDir.x = sPlayerDirection.x;
	this->vCurDir.y = sPlayerDirection.y;
	this->vCurDir.z = sPlayerDirection.z;
}
//Sets Player Last Direction
void PlayersMain::SetPlayerLastDirection(sVECTOR3 sPlayerDirection)
{
	this->vLastDir.x = sPlayerDirection.x;
	this->vLastDir.y = sPlayerDirection.y;
	this->vLastDir.z = sPlayerDirection.z;
}
//Set World ID
void PlayersMain::SetWorldID(int iWorldID)
{
	this->iWorldID = iWorldID;
}
//Set World Tblidx
void PlayersMain::SetWorldTblidx(TBLIDX tblWorld)
{
	this->WorldTblx = tblWorld;
}
//Set Player Name
void PlayersMain::SetPlayerName(string sPlayerName)
{
	this->sPlayerName = sPlayerName;
}
//Set Guild Name
void PlayersMain::SetGuildName(string sGuildName)
{
	this->sGuildName = sGuildName;
}
//Set player is In Tutorial
void PlayersMain::SetPlayerIsInTutorial(bool bReally)
{
	this->isInTutorial = bReally;
}
//Sets Player Handle for Duel
void PlayersMain::SetCharIDForDuel(int CharID)
{
	this->CharIDForDuel = CharID;
}
//Sets Player Thread
void PlayersMain::SetPlayerThread(HANDLE thread)
{
	this->PlayerThread = thread;
}
//Sets Last Player Fight
void PlayersMain::SetLastTimeFight(DWORD value)
{
	this->lastFightTime = value;
}
//Sets Player Fight
void PlayersMain::SetPlayerFight(bool isFight)
{
	this->isfighting = isFight;
}
//Sets Player Damage
void PlayersMain::SetPlayerDamage(int value)
{
	this->SetLastTimeFight(timeGetTime());
	this->SetPlayerFight(true);
	if (this->GetPcProfile()->dwCurLP - value <= 0)
		this->GetPcProfile()->dwCurLP = 0;
	else
		this->GetPcProfile()->dwCurLP -= value;
}
/*
  This will create all player profile using all Classe this function will execute:
  "FillProfileWithInfo" he will fill PcProfile with PC Shape,Level,Name,blabla
  "FillProfileWithAttribute" he will fill PcProfile->avatarAttribute with players Attribute
  "FillCharState" he will fill sCharState to use in anywhere
  "LoadCharInvetory" he will load the respective inventory for this character
  "LoadSkills" here he will fill all Skills information
  "LoadSkillBuff" he will load All active buffs&debuffs
  "LoadHTB" he will Load all HTB info(if you have it)
  "LoadQuickSlot" he will load all QuickSlot
  "SetRPBall" Sets how many RP Ball the player have
*/
void PlayersMain::CreatePlayerProfile()
{
	//Profile
	this->FillProfileWithInfo();
	this->FillProfileWithAttribute();
	this->FillCharState();	
	//Inventory
	this->cPlayerInventory->LoadCharInventory();
	//Skills and QuickSlots
	this->cPlayerSkills->LoadSkills();
	this->cPlayerSkills->LoadSkillBuff();
	this->cPlayerSkills->LoadHTB();
	this->cPlayerSkills->LoadQuickSlot();
	//RP Ball
	this->SetRPBall();
}
//This function send all RP Ball Information
void PlayersMain::SendRpBallInformation()
{
	CNtlPacket packet5(sizeof(sGU_UPDATE_CHAR_RP_BALL_MAX));
	sGU_UPDATE_CHAR_RP_BALL_MAX * maxBall = (sGU_UPDATE_CHAR_RP_BALL_MAX*)packet5.GetPacketData();
	CNtlPacket packet6(sizeof(sGU_UPDATE_CHAR_RP_BALL));
	sGU_UPDATE_CHAR_RP_BALL * ball = (sGU_UPDATE_CHAR_RP_BALL*)packet6.GetPacketData();
	int level = 1;
	if (this->sPlayerProfile->byLevel >= 1 && this->sPlayerProfile->byLevel <= 9)
		this->RPCounter = 1;
	else if (this->sPlayerProfile->byLevel >= 20 && this->sPlayerProfile->byLevel <= 19)
		this->RPCounter = 2;
	else if (this->sPlayerProfile->byLevel >= 30 && this->sPlayerProfile->byLevel <= 39)
		this->RPCounter = 3;
	else if (this->sPlayerProfile->byLevel >= 40 && this->sPlayerProfile->byLevel <= 49)
		this->RPCounter = 4;
	else if (this->sPlayerProfile->byLevel >= 50 && this->sPlayerProfile->byLevel <= 59)
		this->RPCounter = 5;
	else if (this->sPlayerProfile->byLevel >= 60 && this->sPlayerProfile->byLevel <= 69)
		this->RPCounter = 6;
	else if (this->sPlayerProfile->byLevel >= 70)
		this->RPCounter = 7;

	maxBall->byMaxRPBall = this->RPCounter;
	maxBall->handle = this->GetAvatarHandle();
	maxBall->wOpCode = GU_UPDATE_CHAR_RP_BALL_MAX;

	ball->bDropByTime = true;
	ball->byCurRPBall = RPCounter;
	ball->handle = this->GetAvatarHandle();
	ball->wOpCode = GU_UPDATE_CHAR_RP_BALL;

	packet5.SetPacketLen(sizeof(sGU_UPDATE_CHAR_RP_BALL_MAX));
	g_pApp->Send(this->GetSession(), &packet5);

	packet6.SetPacketLen(sizeof(sGU_UPDATE_CHAR_RP_BALL));
	g_pApp->Send(this->GetAvatarHandle(), &packet6);
}
void PlayersMain::SendRpBallUpdate(int moreOrLess)
{
	CNtlPacket packet6(sizeof(sGU_UPDATE_CHAR_RP_BALL));
	sGU_UPDATE_CHAR_RP_BALL * ball = (sGU_UPDATE_CHAR_RP_BALL*)packet6.GetPacketData();

	if (moreOrLess == 0)
	{
		if (this->GetRpBallFull() <= this->GetRpBallCounter())
		{
			this->SetRpBallFull(this->GetRpBallFull() + 1);
			ball->bDropByTime = true;
		}
	}
	else
	{
		if (this->GetRpBallFull() >= 1)
		{
			this->SetRpBallFull(this->GetRpBallFull() - 1);
			ball->bDropByTime = false;
		}
	}
	ball->byCurRPBall = this->GetRpBallFull();
	ball->handle = this->GetAvatarHandle();
	ball->wOpCode = GU_UPDATE_CHAR_RP_BALL;
	packet6.SetPacketLen(sizeof(sGU_UPDATE_CHAR_RP_BALL));
	g_pApp->Send(this->GetSession(), &packet6);
}
//This function will save all player data in Database
void PlayersMain::SavePlayerData(CGameServer* app)
{
	MySQLConnWrapper *db = new MySQLConnWrapper;
	db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
	db->connect();
	db->switchDb(app->GetConfigFileDatabase());

	char* save_query = "UPDATE characters SET CurLocX=? , CurLocY=? , CurLocZ=? , CurDirX=? , CurDirZ=? , level=?, exp=?, MaxExpInThisLevel=?,WorldTable=?, WorldID=?, money=?, MoneyBank=?, reputation=?, MudosaPoint=?, SpPoint=?, BaseStr=?, LastStr=?, BaseCon=?, LastCon=?, BaseFoc=?, LastFoc=?, BaseDex=?, LastDex=?, BaseSol=?, LastSol=?, BaseEng=?, LastEng=?, BaseMaxLP=?, LastMaxLP=?, BaseMaxRp=?, LastMaxRP=? WHERE CharID = ?";
	char* save_query2 = "UPDATE characters SET BaseLpRegen=?, LastLpRegen=?, BaseLpSitdownRegen=?, LastLpSitdownRegen=?, BaseLpBattleRegen=?, LastLpBattleRegen=?, BaseEpRegen=?, LastEpRegen=?, BaseEpSitdownRegen=?, LastEpSitdownRegen=?, BaseEpBattleRegen=?, LastEpBattleRegen=?, BaseRpRegen=?, LastRpRegen=?, LastRpDimimutionRate=?, BasePhysicalOffence=?, LastPhysicalOffence=?, BasePhysicalDefence=? WHERE CharID = ?";
	char* save_query3 = "UPDATE characters SET LastPhysicalDefence=?, BaseEnergyOffence=?, LastEnergyOffence=?, BaseEnergyDefence=?, LastEnergyDefence=?, BaseAttackRate=?, LastAttackRate=?,  BaseDodgeRate=?, LastDodgeRate=?, BaseBlockRate=?, BaseBlockRate=?, LastBlockRate=?, BaseCurseSuccessRate=?, LastCurseSuccessRate=?, BaseCurseToleranceRate=?, LastCurseToleranceRate=?, BasePhysicalCriticalRate=?, LastPhysicalCriticalRate=? WHERE CharID = ?";
	char* save_query4 = "UPDATE characters SET BaseEnergyCriticalRate=?, LastEnergyCriticalRate=?, LastRunSpeed=?, BaseAttackSpeedRate=?, BaseAttackRange=?, LastAttackRange=?, CastingTimeChangePercent=?, CoolTimeChangePercent=?, KeepTimeChangePercent=?, DotValueChangePercent=?, DotTimeChangeAbsolute=?, RequiredEpChangePercent=?, HonestOffence=?, HonestDefence=?, StrangeOffence=?, StrangeDefence=?, WildOffence=?, WildDefence=? WHERE CharID = ?";
	char* save_query5 = "UPDATE characters SET EleganceOffence=?, EleganceDefence=?, FunnyDefence=?, FunnyOffence=?, FunnyDefence=?, ParalyzeToleranceRate=?, TerrorToleranceRate=?, ConfuseToleranceRate=?, StoneToleranceRate=?, CandyToleranceRate=?, ParalyzeKeepTimeDown=?, TerrorKeepTimeDown=?, ConfuseKeepTimeDown=?, StoneKeepTimeDown=?, CandyKeepTimeDown=?, BleedingKeepTimeDown=?, PoisonKeepTimeDown=?, StomachacheKeepTimeDown=?, CriticalBlockSuccessRate=?  WHERE CharID = ?";
	char* save_query6 = "UPDATE characters SET GuardRate=?, SkillDamageBlockModeSuccessRate=?, CurseBlockModeSuccessRate=?, KnockdownBlockModeSuccessRate=?, HtbBlockModeSuccessRate=?, SitDownLpRegenBonusRate=?, SitDownEpRegenBonusRate=?, PhysicalCriticalDamageBonusRate=?, EnergyCriticalDamageBonusRate=?, ItemUpgradeBonusRate=?, ItemUpgradeBreakBonusRate=?, CurLP=?, CurEP=?, CurRP=? WHERE CharID = ?";

	db->prepare(save_query);
	db->setFloat(1, this->vCurLoc.x);
	db->setFloat(2, this->vCurLoc.y);
	db->setFloat(3, this->vCurLoc.z);
	db->setFloat(4, this->vCurDir.x);
	db->setFloat(5, this->vCurDir.z);
	db->setInt(6, this->GetPcProfile()->byLevel);
	db->setInt(7, this->GetPcProfile()->dwCurExp);
	db->setInt(8, this->GetPcProfile()->dwMaxExpInThisLevel);
	db->setInt(9, this->GetWorldTblx());
	db->setInt(10, this->GetWorldID());
	db->setInt(11, this->GetPcProfile()->dwZenny);
	db->setInt(12, this->GetPcProfile()->dwZenny);//bank
	db->setInt(13, this->GetPcProfile()->dwReputation);
	db->setInt(14, this->GetPcProfile()->dwMudosaPoint);
	db->setInt(15, this->GetPcProfile()->dwSpPoint);
	db->setInt(16, this->GetPcProfile()->avatarAttribute.byBaseStr);
	db->setInt(17, this->GetPcProfile()->avatarAttribute.byLastStr);
	db->setInt(18, this->GetPcProfile()->avatarAttribute.byBaseCon);
	db->setInt(19, this->GetPcProfile()->avatarAttribute.byLastCon);
	db->setInt(20, this->GetPcProfile()->avatarAttribute.byBaseFoc);
	db->setInt(21, this->GetPcProfile()->avatarAttribute.byLastFoc);
	db->setInt(22, this->GetPcProfile()->avatarAttribute.byBaseDex);
	db->setInt(23, this->GetPcProfile()->avatarAttribute.byLastDex);
	db->setInt(24, this->GetPcProfile()->avatarAttribute.byBaseSol);
	db->setInt(25, this->GetPcProfile()->avatarAttribute.byLastSol);
	db->setInt(26, this->GetPcProfile()->avatarAttribute.byBaseEng);
	db->setInt(27, this->GetPcProfile()->avatarAttribute.byLastEng);
	db->setInt(28, this->GetPcProfile()->avatarAttribute.wBaseMaxLP);
	db->setInt(29, this->GetPcProfile()->avatarAttribute.wLastMaxLP);
	db->setInt(30, this->GetPcProfile()->avatarAttribute.wBaseMaxRP);
	db->setInt(31, this->GetPcProfile()->avatarAttribute.wLastMaxRP);
	db->setInt(32, this->GetPcProfile()->charId);
	db->execute();

	// 1 DONE
	db->prepare(save_query2);
	db->setInt(1, this->GetPcProfile()->avatarAttribute.wBaseLpRegen);
	db->setInt(2, this->GetPcProfile()->avatarAttribute.wLastLpRegen);
	db->setInt(3, this->GetPcProfile()->avatarAttribute.wBaseLpSitdownRegen);
	db->setInt(4, this->GetPcProfile()->avatarAttribute.wLastLpSitdownRegen);
	db->setInt(5, this->GetPcProfile()->avatarAttribute.wBaseLpBattleRegen);
	db->setInt(6, this->GetPcProfile()->avatarAttribute.wLastLpBattleRegen);
	db->setInt(7, this->GetPcProfile()->avatarAttribute.wBaseEpRegen);
	db->setInt(8, this->GetPcProfile()->avatarAttribute.wLastEpRegen);
	db->setInt(9, this->GetPcProfile()->avatarAttribute.wBaseEpSitdownRegen);
	db->setInt(10, this->GetPcProfile()->avatarAttribute.wLastEpSitdownRegen);
	db->setInt(11, this->GetPcProfile()->avatarAttribute.wBaseEpBattleRegen);
	db->setInt(12, this->GetPcProfile()->avatarAttribute.wLastEpBattleRegen);
	db->setInt(13, this->GetPcProfile()->avatarAttribute.wBaseRpRegen);
	db->setInt(14, this->GetPcProfile()->avatarAttribute.wLastRpRegen);
	db->setInt(15, this->GetPcProfile()->avatarAttribute.wLastRpDimimutionRate);
	db->setInt(16, this->GetPcProfile()->avatarAttribute.wBasePhysicalOffence);
	db->setInt(17, this->GetPcProfile()->avatarAttribute.wLastPhysicalOffence);
	db->setInt(18, this->GetPcProfile()->avatarAttribute.wBasePhysicalDefence);
	db->setInt(19, this->GetPcProfile()->charId);
	db->execute();

	// 2 DONE
	db->prepare(save_query3);
	db->setInt(1, this->GetPcProfile()->avatarAttribute.wLastPhysicalDefence);
	db->setInt(2, this->GetPcProfile()->avatarAttribute.wBaseEnergyOffence);
	db->setInt(3, this->GetPcProfile()->avatarAttribute.wLastEnergyOffence);
	db->setInt(4, this->GetPcProfile()->avatarAttribute.wBaseEnergyDefence);
	db->setInt(5, this->GetPcProfile()->avatarAttribute.wLastEnergyDefence);
	db->setInt(6, this->GetPcProfile()->avatarAttribute.wBaseAttackRate);
	db->setInt(7, this->GetPcProfile()->avatarAttribute.wLastAttackRate);
	db->setInt(8, this->GetPcProfile()->avatarAttribute.wBaseDodgeRate);
	db->setInt(9, this->GetPcProfile()->avatarAttribute.wLastDodgeRate);
	db->setInt(10, this->GetPcProfile()->avatarAttribute.wBaseBlockRate);
	db->setInt(11, this->GetPcProfile()->avatarAttribute.wBaseBlockRate);
	db->setInt(12, this->GetPcProfile()->avatarAttribute.wLastBlockRate);
	db->setInt(13, this->GetPcProfile()->avatarAttribute.wBaseCurseSuccessRate);
	db->setInt(14, this->GetPcProfile()->avatarAttribute.wLastCurseSuccessRate);
	db->setInt(15, this->GetPcProfile()->avatarAttribute.wBaseCurseToleranceRate);
	db->setInt(16, this->GetPcProfile()->avatarAttribute.wLastCurseToleranceRate);
	db->setInt(17, this->GetPcProfile()->avatarAttribute.wBasePhysicalCriticalRate);
	db->setInt(18, this->GetPcProfile()->avatarAttribute.wLastPhysicalCriticalRate);
	db->setInt(19, this->GetPcProfile()->charId);
	db->execute();

	//3 DONE
	db->prepare(save_query4);
	db->setInt(1, this->GetPcProfile()->avatarAttribute.wBaseEnergyCriticalRate);
	db->setInt(2, this->GetPcProfile()->avatarAttribute.wLastEnergyCriticalRate);
	db->setFloat(3, this->GetPcProfile()->avatarAttribute.fLastRunSpeed);
	db->setInt(4, this->GetPcProfile()->avatarAttribute.wBaseAttackSpeedRate);
	db->setFloat(5, this->GetPcProfile()->avatarAttribute.fBaseAttackRange);
	db->setFloat(6, this->GetPcProfile()->avatarAttribute.fLastAttackRange);
	db->setFloat(7, this->GetPcProfile()->avatarAttribute.fCastingTimeChangePercent);
	db->setFloat(8, this->GetPcProfile()->avatarAttribute.fCoolTimeChangePercent);
	db->setFloat(9, this->GetPcProfile()->avatarAttribute.fKeepTimeChangePercent);
	db->setFloat(10, this->GetPcProfile()->avatarAttribute.fDotValueChangePercent);
	db->setFloat(11, this->GetPcProfile()->avatarAttribute.fDotTimeChangeAbsolute);
	db->setFloat(12, this->GetPcProfile()->avatarAttribute.fRequiredEpChangePercent);
	db->setFloat(13, this->GetPcProfile()->avatarAttribute.fHonestOffence);
	db->setFloat(14, this->GetPcProfile()->avatarAttribute.fHonestDefence);
	db->setFloat(15, this->GetPcProfile()->avatarAttribute.fStrangeOffence);
	db->setFloat(16, this->GetPcProfile()->avatarAttribute.fStrangeDefence);
	db->setFloat(17, this->GetPcProfile()->avatarAttribute.fWildOffence);
	db->setFloat(18, this->GetPcProfile()->avatarAttribute.fWildDefence);
	db->setInt(19, this->GetPcProfile()->charId);
	db->execute();
	
	//4 DONE
	db->prepare(save_query5);
	db->setFloat(1, this->GetPcProfile()->avatarAttribute.fEleganceOffence);
	db->setFloat(2, this->GetPcProfile()->avatarAttribute.fEleganceDefence);
	db->setFloat(3, this->GetPcProfile()->avatarAttribute.fFunnyDefence);
	db->setFloat(4, this->GetPcProfile()->avatarAttribute.fFunnyOffence);
	db->setFloat(5, this->GetPcProfile()->avatarAttribute.fFunnyDefence);
	db->setInt(6, this->GetPcProfile()->avatarAttribute.wParalyzeToleranceRate);
	db->setInt(7, this->GetPcProfile()->avatarAttribute.wTerrorToleranceRate);
	db->setInt(8, this->GetPcProfile()->avatarAttribute.wConfuseToleranceRate);
	db->setInt(9, this->GetPcProfile()->avatarAttribute.wStoneToleranceRate);
	db->setInt(10, this->GetPcProfile()->avatarAttribute.wCandyToleranceRate);
	db->setFloat(11, this->GetPcProfile()->avatarAttribute.fParalyzeKeepTimeDown);
	db->setFloat(12, this->GetPcProfile()->avatarAttribute.fTerrorKeepTimeDown);
	db->setFloat(13, this->GetPcProfile()->avatarAttribute.fConfuseKeepTimeDown);
	db->setFloat(14, this->GetPcProfile()->avatarAttribute.fStoneKeepTimeDown);
	db->setFloat(15, this->GetPcProfile()->avatarAttribute.fCandyKeepTimeDown);
	db->setFloat(16, this->GetPcProfile()->avatarAttribute.fBleedingKeepTimeDown);
	db->setFloat(17, this->GetPcProfile()->avatarAttribute.fPoisonKeepTimeDown);
	db->setFloat(18, this->GetPcProfile()->avatarAttribute.fStomachacheKeepTimeDown);
	db->setFloat(19, this->GetPcProfile()->avatarAttribute.fCriticalBlockSuccessRate);
	db->setInt(20, this->GetPcProfile()->charId);
	db->execute();

	//5 DONE
	db->prepare(save_query6);
	db->setInt(1, this->GetPcProfile()->avatarAttribute.wGuardRate);
	db->setFloat(2, this->GetPcProfile()->avatarAttribute.fSkillDamageBlockModeSuccessRate);
	db->setFloat(3, this->GetPcProfile()->avatarAttribute.fCurseBlockModeSuccessRate);
	db->setFloat(4, this->GetPcProfile()->avatarAttribute.fKnockdownBlockModeSuccessRate);
	db->setFloat(5, this->GetPcProfile()->avatarAttribute.fHtbBlockModeSuccessRate);
	db->setFloat(6, this->GetPcProfile()->avatarAttribute.fSitDownLpRegenBonusRate);
	db->setFloat(7, this->GetPcProfile()->avatarAttribute.fSitDownEpRegenBonusRate);
	db->setFloat(8, this->GetPcProfile()->avatarAttribute.fPhysicalCriticalDamageBonusRate);
	db->setFloat(9, this->GetPcProfile()->avatarAttribute.fEnergyCriticalDamageBonusRate);
	db->setFloat(10, this->GetPcProfile()->avatarAttribute.fItemUpgradeBonusRate);
	db->setFloat(11, this->GetPcProfile()->avatarAttribute.fItemUpgradeBreakBonusRate);
	db->setInt(12, this->GetPcProfile()->dwCurLP);
	db->setInt(13, this->GetPcProfile()->wCurEP);
	db->setInt(14, this->GetPcProfile()->wCurRP);
	db->setInt(15, this->GetPcProfile()->charId);
	db->execute();

	delete db;
}
void PlayersMain::SetStats(sPC_TBLDAT *pTblData)
{
	this->sPcData->byLevel_Up_Energy_Defence = pTblData->byLevel_Up_Energy_Defence;
	this->sPcData->byLevel_Up_Energy_Offence = pTblData->byLevel_Up_Energy_Offence;
	this->sPcData->byLevel_Up_EP = pTblData->byLevel_Up_EP;
	this->sPcData->byLevel_Up_LP = pTblData->byLevel_Up_LP;
	this->sPcData->byLevel_Up_Physical_Defence = pTblData->byLevel_Up_Physical_Defence;
	this->sPcData->byLevel_Up_Physical_Offence = pTblData->byLevel_Up_Physical_Offence;
	this->sPcData->byLevel_Up_RP = pTblData->byLevel_Up_RP;
	this->sPcData->fLevel_Up_Con = pTblData->fLevel_Up_Con;
	this->sPcData->fLevel_Up_Dex = pTblData->fLevel_Up_Dex;
	this->sPcData->fLevel_Up_Eng = pTblData->fLevel_Up_Eng;
	this->sPcData->fLevel_Up_Foc = pTblData->fLevel_Up_Foc;
	this->sPcData->fLevel_Up_Sol = pTblData->fLevel_Up_Sol;
	this->sPcData->fLevel_Up_Str = pTblData->fLevel_Up_Str;
}
//Level UP a player by 1 level
void PlayersMain::SetLevelUP()
{
/*	this->GetPcProfile()->avatarAttribute.wBasePhysicalOffence += (WORD)this->sPcData->byLevel_Up_Physical_Offence;
	this->GetPcProfile()->avatarAttribute.wBasePhysicalDefence += (WORD)this->sPcData->byLevel_Up_Physical_Defence;
	this->GetPcProfile()->avatarAttribute.wBaseEnergyOffence += (WORD)this->sPcData->byLevel_Up_Energy_Offence;
	this->GetPcProfile()->avatarAttribute.wBaseEnergyDefence += (WORD)this->sPcData->byLevel_Up_Energy_Defence;
	this->GetPcProfile()->avatarAttribute.byBaseCon += (BYTE)this->sPcData->fLevel_Up_Con;
	this->GetPcProfile()->avatarAttribute.byBaseDex += (BYTE)this->sPcData->fLevel_Up_Dex;
	this->GetPcProfile()->avatarAttribute.byBaseEng += (BYTE)this->sPcData->fLevel_Up_Eng;
	this->GetPcProfile()->avatarAttribute.byBaseFoc += (BYTE)this->sPcData->fLevel_Up_Foc;
	this->GetPcProfile()->avatarAttribute.byBaseSol += (BYTE)this->sPcData->fLevel_Up_Sol;
	this->GetPcProfile()->avatarAttribute.byBaseStr += (BYTE)this->sPcData->fLevel_Up_Str;
	this->GetPcProfile()->avatarAttribute.wBaseMaxRP += (WORD)this->sPcData->byLevel_Up_RP;
	this->GetPcProfile()->avatarAttribute.wBaseMaxEP = (WORD)(this->GetPcProfile()->avatarAttribute.wBaseMaxEP + this->sPcData->byLevel_Up_EP);
	this->GetPcProfile()->avatarAttribute.wBaseMaxEP = this->GetPcProfile()->avatarAttribute.wBaseMaxEP + (WORD)(this->GetPcProfile()->avatarAttribute.byLastEng * 4.2);
	this->GetPcProfile()->avatarAttribute.wBaseMaxLP = (this->GetPcProfile()->avatarAttribute.wBaseMaxLP + (WORD)this->sPcData->byLevel_Up_LP);
	this->GetPcProfile()->avatarAttribute.wBaseMaxLP = this->GetPcProfile()->avatarAttribute.wBaseMaxLP + (WORD)(this->GetPcProfile()->avatarAttribute.byLastCon* 4.7);
	*/
	this->SetRPBall();
	this->cPlayerAttribute->LoadAttributesFromDB(this->GetCharID());
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_LP_EP));
	sGU_UPDATE_CHAR_LP_EP * res = (sGU_UPDATE_CHAR_LP_EP *)packet.GetPacketData();
	this->GetPcProfile()->wCurEP = this->GetPcProfile()->avatarAttribute.wBaseMaxEP;
	this->GetPcProfile()->dwCurLP = this->GetPcProfile()->avatarAttribute.wBaseMaxLP;

	//res->handle = this->avatarHandle;
	//res->wCurEP = this->GetPcProfile()->wCurEP;
	//res->wCurLP = this->GetPcProfile()->dwCurLP;
	//res->wMaxEP = this->GetPcProfile()->wCurEP;
	//res->wMaxLP = this->GetPcProfile()->dwCurLP;
	//res->wOpCode = GU_UPDATE_CHAR_LP_EP;
	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LP_EP));
	g_pApp->Send(this->GetSession(), &packet);
}
//Update Attributes with equipped value from equipment
void PlayersMain::UpdateBaseAttributeWithEquip(TBLIDX itemTBL,BYTE byRank,BYTE byGrade,bool bRemove)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	CItemTable *itemTbl = app->g_pTableContainer->GetItemTable();
	CItemOptionTable* itemOptionTbl = app->g_pTableContainer->GetItemOptionTable();
	sITEM_TBLDAT* pItemData = reinterpret_cast<sITEM_TBLDAT*>(itemTbl->FindData(itemTBL));
	sITEM_DATA* pDataItem = (sITEM_DATA*)pItemData->tblidx;
	sITEM_DATA* pDataItem23 = (sITEM_DATA*)pItemData->Name;
	if (pItemData->wPhysical_Offence < 65535 && pItemData->wPhysical_Offence > 0)
	{
		if (bRemove)
			this->sPlayerProfile->avatarAttribute.wLastPhysicalOffence -= Dbo_GetFinalOffence(pItemData->wPhysical_Offence, byGrade);
		else
			this->sPlayerProfile->avatarAttribute.wLastPhysicalOffence += Dbo_GetFinalOffence(pItemData->wPhysical_Offence, byGrade);
	}
	if (pItemData->wPhysical_Defence < 65535 && pItemData->wPhysical_Defence > 0)
	{
		if (bRemove)
			this->sPlayerProfile->avatarAttribute.wLastPhysicalDefence -= Dbo_GetFinalDefence(pItemData->wPhysical_Defence, byGrade);
		else
			this->sPlayerProfile->avatarAttribute.wLastPhysicalDefence += Dbo_GetFinalDefence(pItemData->wPhysical_Defence, byGrade);
	}
	if (pItemData->wEnergy_Offence < 65535 && pItemData->wEnergy_Offence > 0)
	{
		if (bRemove)
			this->sPlayerProfile->avatarAttribute.wLastEnergyOffence -= Dbo_GetFinalOffence(pItemData->wEnergy_Offence, byGrade);
		else
			this->sPlayerProfile->avatarAttribute.wLastEnergyOffence += Dbo_GetFinalOffence(pItemData->wEnergy_Offence, byGrade);
	}
	if (pItemData->wEnergy_Defence < 65535 && pItemData->wEnergy_Defence > 0)
	{
		if (bRemove)
			this->sPlayerProfile->avatarAttribute.wLastEnergyDefence -= Dbo_GetFinalDefence(pItemData->wEnergy_Defence, byGrade);
		else
			this->sPlayerProfile->avatarAttribute.wLastEnergyDefence += Dbo_GetFinalDefence(pItemData->wEnergy_Defence, byGrade);
	}
	if (pItemData->wAttack_Speed_Rate < 65535 && pItemData->wAttack_Speed_Rate > 0)
	{	
		this->sPlayerProfile->avatarAttribute.wLastAttackSpeedRate += pItemData->wAttack_Speed_Rate;
	}
	if (pItemData->fAttack_Range_Bonus < 65535 && pItemData->fAttack_Range_Bonus > 0)
	{
		this->sPlayerProfile->avatarAttribute.fLastAttackRange += pItemData->fAttack_Range_Bonus;
	}
}
//Send Updates for LP and EP of Player Thread
void PlayersMain::SendThreadUpdateEPLP()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_LP_EP));
	sGU_UPDATE_CHAR_LP_EP * res = (sGU_UPDATE_CHAR_LP_EP *)packet.GetPacketData();

	res->handle = this->avatarHandle;
	res->wCurEP = this->GetPcProfile()->wCurEP;
	res->wCurLP = this->GetPcProfile()->dwCurLP;
	res->wMaxEP = this->GetPcProfile()->avatarAttribute.wBaseMaxEP;
	res->wMaxLP = this->GetPcProfile()->avatarAttribute.wBaseMaxLP;
	res->wOpCode = GU_UPDATE_CHAR_LP_EP;

	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LP_EP));
	g_pApp->Send(this->GetSession(), &packet);
	app->UserBroadcastothers(&packet, this->myCCSession);
}
//Send Updates for EP Only EP of Player Thread
void PlayersMain::SendThreadUpdateOnlyEP()
{
	if (this->GetPcProfile()->avatarAttribute.wBaseEpRegen <= 0)
		this->GetPcProfile()->avatarAttribute.wBaseEpRegen = (WORD)(this->GetPcProfile()->avatarAttribute.wBaseMaxEP * 0.01);

	this->GetPcProfile()->wCurEP += this->GetPcProfile()->avatarAttribute.wBaseEpRegen; // += regen
	if (this->GetPcProfile()->wCurEP > this->GetPcProfile()->avatarAttribute.wBaseMaxEP)
		this->GetPcProfile()->wCurEP = this->GetPcProfile()->avatarAttribute.wBaseMaxEP;
}
//Send Updates for LP Only LP of Player Thread
void PlayersMain::SendThreadUpdateOnlyLP()
{
	if (this->GetPcProfile()->avatarAttribute.wBaseLpRegen <= 0)
		this->GetPcProfile()->avatarAttribute.wBaseLpRegen = (WORD)(this->GetPcProfile()->avatarAttribute.wBaseMaxLP * 0.01);
	else
	{
		this->GetPcProfile()->dwCurLP += this->GetPcProfile()->avatarAttribute.wBaseLpRegen; // += regen
		if (this->GetPcProfile()->dwCurLP > this->GetPcProfile()->avatarAttribute.wBaseMaxLP)
			this->GetPcProfile()->dwCurLP = this->GetPcProfile()->avatarAttribute.wBaseMaxLP;
	}
}
//Send Updates for RP Ball of Player Thread
void PlayersMain::SendThreadUpdateRP()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_RP));
	sGU_UPDATE_CHAR_RP * res = (sGU_UPDATE_CHAR_RP *)packet.GetPacketData();

	res->bHitDelay = false;
	res->handle = this->avatarHandle;
	res->wCurRP = this->GetPcProfile()->wCurRP;
	if (GetRpBallCounter() >= 1)
		res->wMaxRP = (this->GetPcProfile()->avatarAttribute.wBaseMaxRP / this->GetRpBallCounter());
	else
		res->wMaxRP = this->GetPcProfile()->avatarAttribute.wBaseMaxRP;
	res->wOpCode = GU_UPDATE_CHAR_RP;
	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_RP));
	g_pApp->Send(this->GetSession(), &packet);
	app->UserBroadcastothers(&packet, this->myCCSession);
}
//Send Updates to see if the player already dead
void PlayersMain::SendThreadUpdateDeathStatus()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	this->SetPlayerFight(false);
	this->SetPlayerDead(true);
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE * res = (sGU_UPDATE_CHAR_STATE *)packet.GetPacketData();

	res->wOpCode = GU_UPDATE_CHAR_STATE;
	res->handle = this->avatarHandle;
	res->sCharState.sCharStateBase.byStateID = CHARSTATE_FAINTING;
	res->sCharState.sCharStateBase.vCurLoc = this->GetPlayerPosition();
	res->sCharState.sCharStateBase.vCurDir = this->GetPlayerDirection();

	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
	app->UserBroadcastothers(&packet, this->myCCSession);
	g_pApp->Send(this->GetSession(), &packet);
}
//Send Thread Revival Status
void PlayersMain::SendThreadRevivalStatus()
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	this->SetPlayerDead(false);

	this->GetPcProfile()->dwCurLP = this->GetPcProfile()->avatarAttribute.wBaseMaxLP;
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE * res = (sGU_UPDATE_CHAR_STATE *)packet.GetPacketData();

	res->wOpCode = GU_UPDATE_CHAR_STATE;
	res->handle = this->avatarHandle;
	res->sCharState.sCharStateBase.byStateID = CHARSTATE_STANDING;
	res->sCharState.sCharStateBase.vCurLoc = this->GetPlayerPosition();
	res->sCharState.sCharStateBase.vCurDir = this->GetPlayerDirection();

	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
	app->UserBroadcastothers(&packet, this->myCCSession);
	g_pApp->Send(this->GetSession(), &packet);

}
//Update For Player Thread maybe this is the Real Player Thread
DWORD WINAPI Update(LPVOID arg)
{
	DWORD dwTickCur, dwTickOld = ::GetTickCount();
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	PlayersMain* plr = (PlayersMain*)arg;
	if (plr)
	{
		while (true)
		{
			if (plr->GetPlayerDead() == false)
			{
				dwTickCur = ::GetTickCount();
				if (plr->GetPlayerFight() == true)
				{
					
					printf("I'm fighting.\n");
					if (dwTickCur - dwTickOld >= 10000)
					{
						plr->SetPlayerFight(false);
						dwTickOld = dwTickCur;
					}
				}
				else if (plr->GetPlayerFight() == false)
				{
					if (plr->GetPcProfile()->dwCurLP <= 0)
						plr->SendThreadUpdateDeathStatus();
					else if (plr->GetPcProfile()->dwCurLP < plr->GetPcProfile()->avatarAttribute.wBaseMaxLP || plr->GetPcProfile()->dwCurLP > plr->GetPcProfile()->avatarAttribute.wBaseMaxLP)
						plr->SendThreadUpdateOnlyLP();
					if (plr->GetPcProfile()->wCurEP < plr->GetPcProfile()->avatarAttribute.wBaseMaxEP || plr->GetPcProfile()->wCurEP > plr->GetPcProfile()->avatarAttribute.wBaseMaxEP)
						plr->SendThreadUpdateOnlyEP();
				}
				if ((plr->GetPcProfile()->wCurRP > 0) || plr->GetRpBallFull() > 0)
				{
					if (plr->GetPcProfile()->wCurRP <= 0)
						if (plr->GetRpBallFull() > 0)
						{
						plr->SetRpBallFull(1);
						plr->GetPcProfile()->wCurRP = (plr->GetPcProfile()->avatarAttribute.wBaseMaxRP / plr->GetRpBallCounter()) - 1;
						}
						else;
					else
						plr->GetPcProfile()->wCurRP -= 1;
					plr->SendThreadUpdateRP();
				}
				/*if (plr->isKaioken == true) /* TEST */
				/*{
					plr->GetPcProfile()->wCurLP -= (500 * plr->GetCharState()->sCharStateBase.aspectState.sAspectStateDetail.sKaioken.byRepeatingCount);
					plr->GetPcProfile()->wCurEP -= (500 * plr->GetCharState()->sCharStateBase.aspectState.sAspectStateDetail.sKaioken.byRepeatingCount);
				}*/
				//plr->SendThreadUpdateEPLP();
				if ((timeGetTime() - plr->GetMob_SpawnTime()) >= MONSTER_SPAWN_UPDATE_TICK)
				{
					//app->mob->RunSpawnCheck(NULL, plr->GetPlayerPosition(), plr->myCCSession);
					//plr->SetMob_SpawnTime(timeGetTime());
				}
			}
			plr = plr->GetRefreshPointer();
			Sleep(1000);// And no it's every second, it's only the amount regen is too high (this->pcProfile->avatarAttribute.wBaseMaxEP * 0.03) 3% every seconds it's for make some test this is not the last "release"			
		}
	}
	return 0;
}
//Sets Player Thread ID and Already set and run the thread
void PlayersMain::SetPlayerThreadID(DWORD id)
{
	this->PlayerThreadID = id;
	this->SetPlayerThread(CreateThread(NULL, 0, Update, (LPVOID)this, 0, &id));
}