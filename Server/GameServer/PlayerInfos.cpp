#include "stdafx.h"
#include "GameServer.h"
#include "Avatar.h"

void		PlayerInfos::SendPlayerLifeAndEP()
{
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_LP_EP));
	sGU_UPDATE_CHAR_LP_EP * res = (sGU_UPDATE_CHAR_LP_EP *)packet.GetPacketData();

	res->handle = this->avatarHandle;
	res->wCurEP = this->pcProfile->wCurEP;
	res->wCurLP = this->pcProfile->wCurLP;
	res->wMaxEP = this->pcProfile->avatarAttribute.wBaseMaxEP;
	res->wMaxLP = this->pcProfile->avatarAttribute.wBaseMaxLP;
	res->wOpCode = GU_UPDATE_CHAR_LP_EP;

	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_LP_EP));
	g_pApp->Send(this->MySession, &packet);
	app->UserBroadcastothers(&packet, this->myCCSession);
}
void		PlayerInfos::UpdateRevivalStatus()
{
	this->pcProfile->wCurLP = this->pcProfile->avatarAttribute.wBaseMaxLP;
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE * res = (sGU_UPDATE_CHAR_STATE *)packet.GetPacketData();

	res->wOpCode = GU_UPDATE_CHAR_STATE;
	res->handle = this->avatarHandle;
	res->sCharState.sCharStateBase.byStateID = CHARSTATE_STANDING;
	res->sCharState.sCharStateBase.vCurLoc = this->GetPosition();
	res->sCharState.sCharStateBase.vCurDir = this->GetDirection();

	packet.SetPacketLen( sizeof(sGU_UPDATE_CHAR_STATE) );
	app->UserBroadcastothers(&packet, this->myCCSession);
	g_pApp->Send( this->MySession , &packet );
	this->isdead = false;
}
void		PlayerInfos::UpdateDeathStatus()
{
	this->setFightMod(false);
	this->isdead = true;
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
	sGU_UPDATE_CHAR_STATE * res = (sGU_UPDATE_CHAR_STATE *)packet.GetPacketData();

	res->wOpCode = GU_UPDATE_CHAR_STATE;
	res->handle = this->avatarHandle;
	res->sCharState.sCharStateBase.byStateID = CHARSTATE_FAINTING;
	res->sCharState.sCharStateBase.vCurLoc = this->GetPosition();
	res->sCharState.sCharStateBase.vCurDir = this->GetDirection();

	packet.SetPacketLen( sizeof(sGU_UPDATE_CHAR_STATE) );
	app->UserBroadcastothers(&packet, this->myCCSession);
	g_pApp->Send( this->MySession , &packet );
}
void		PlayerInfos::TakeDamage(int Damage)
{
	this->lastFightTime = timeGetTime();
	this->setFightMod(true);
	if (this->pcProfile->wCurLP - Damage <= 0)
		this->pcProfile->wCurLP = 0;
	else
		this->pcProfile->wCurLP -= Damage;
}
void		PlayerInfos::UpdateRP()
{
	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_RP));
	sGU_UPDATE_CHAR_RP * res = (sGU_UPDATE_CHAR_RP *)packet.GetPacketData();
	
	res->bHitDelay = false;
	res->handle = this->avatarHandle;
	res->wCurRP = this->pcProfile->wCurRP;
	if (getNumberOfRPBall() >= 1)
		res->wMaxRP = (this->pcProfile->avatarAttribute.wBaseMaxRP / this->getNumberOfRPBall());
	else
		res->wMaxRP = this->pcProfile->avatarAttribute.wBaseMaxRP;
	res->wOpCode = GU_UPDATE_CHAR_RP;
	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_RP));
	g_pApp->Send(this->MySession, &packet);
	app->UserBroadcastothers(&packet, this->myCCSession);
}
void		PlayerInfos::UpdateLP()
{
	if (this->pcProfile->avatarAttribute.wBaseLpRegen <= 0)
		this->pcProfile->avatarAttribute.wBaseLpRegen = (WORD)(this->pcProfile->avatarAttribute.wBaseMaxLP * 0.01);
	else
	{
		this->pcProfile->wCurLP += this->pcProfile->avatarAttribute.wBaseLpRegen; // += regen
		if (this->pcProfile->wCurLP > this->pcProfile->avatarAttribute.wBaseMaxLP)
			this->pcProfile->wCurLP = this->pcProfile->avatarAttribute.wBaseMaxLP;
	}
}

void	    PlayerInfos::UpdateEP()
{
	if (this->pcProfile->avatarAttribute.wBaseEpRegen <= 0)
		this->pcProfile->avatarAttribute.wBaseEpRegen = (WORD)(this->pcProfile->avatarAttribute.wBaseMaxEP * 0.01);

	this->pcProfile->wCurEP += this->pcProfile->avatarAttribute.wBaseEpRegen; // += regen
	if (this->pcProfile->wCurEP > this->pcProfile->avatarAttribute.wBaseMaxEP)
		this->pcProfile->wCurEP = this->pcProfile->avatarAttribute.wBaseMaxEP;
}
void		PlayerInfos::checkBuff(int skill)
{
	if (skill == 120103 || skill == 120101 || skill == 120102)
	{
		this->pcProfile->avatarAttribute.fLastRunSpeed += 2;
		this->isKaioken = true;
		this->sCharState->sCharStateBase.aspectState.sAspectStateDetail.sKaioken.byRepeatingCount += 1;
	}
}
DWORD WINAPI	UpdatePlayer(LPVOID arg)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();
	PlayerInfos* plr = (PlayerInfos*)arg;
	if (plr)
	{
		while (true)
		{
			if (plr->getDeadMod() == false)
			{
				if (plr->getFightMod() == true)
				{
					printf("I'm fighting.\n");
					/*if (timeGetTime() >= plr->lastFightTime + 5000)
						plr->isfighting = false;*/
				}
				else if (plr->getFightMod() == false)
				{
					if (plr->pcProfile->wCurLP <= 0)
						plr->UpdateDeathStatus();
					else if (plr->pcProfile->wCurLP < plr->pcProfile->avatarAttribute.wBaseMaxLP || plr->pcProfile->wCurLP > plr->pcProfile->avatarAttribute.wBaseMaxLP)
						plr->UpdateLP();
					if (plr->pcProfile->wCurEP < plr->pcProfile->avatarAttribute.wBaseMaxEP || plr->pcProfile->wCurEP > plr->pcProfile->avatarAttribute.wBaseMaxEP)
						plr->UpdateEP();
				}
				if ((plr->pcProfile->wCurRP > 0) || plr->getRpBallOk() > 0)
				{
					if (plr->pcProfile->wCurRP <= 0)
						if (plr->getRpBallOk() > 0)
						{
							plr->UpdateRpBallOk(1);
							plr->pcProfile->wCurRP = (plr->pcProfile->avatarAttribute.wBaseMaxRP / plr->getNumberOfRPBall()) - 1;
						}
						else;
					else
						plr->pcProfile->wCurRP -= 1;
					plr->UpdateRP();
				}
				if (plr->isKaioken == true) /* TEST */
				{
					plr->pcProfile->wCurLP -= (500 * plr->sCharState->sCharStateBase.aspectState.sAspectStateDetail.sKaioken.byRepeatingCount);
					plr->pcProfile->wCurEP -= (500 * plr->sCharState->sCharStateBase.aspectState.sAspectStateDetail.sKaioken.byRepeatingCount);
				}
				plr->SendPlayerLifeAndEP();
				if((timeGetTime() - plr->Getmob_SpawnTime()) >= MONSTER_SPAWN_UPDATE_TICK)
				{
					//app->mob->RunSpawnCheck(NULL, plr->GetPosition(), plr->myCCSession);
					plr->Setmob_SpawnTime(timeGetTime());
				}
			}
			plr = plr->refreshPointer();
			Sleep(1000);// And no it's every second, it's only the amount regen is too high (this->pcProfile->avatarAttribute.wBaseMaxEP * 0.03) 3% every seconds it's for make some test this is not the last "release"
		}
	}
	return 0;
}
void		PlayerInfos::SpawnMyChar()
{
	this->dwThreadId = this->app->ThreadRequest();
	this->hThread = CreateThread(NULL, 0, UpdatePlayer, (LPVOID)this, 0, &this->dwThreadId);
	if (this->hThread == NULL)
        printf("Can't create thread Regen\n");
};
void		PlayerInfos::SavePlayerData()
{
	this->db = new MySQLConnWrapper;
	this->db->setConfig(this->app->GetConfigFileHost(), this->app->GetConfigFileUser(), this->app->GetConfigFilePassword(), this->app->GetConfigFileDatabase());
	this->db->connect();
	this->db->switchDb(this->app->GetConfigFileDatabase());

	char* save_query = "UPDATE characters SET CurLocX=? , CurLocY=? , CurLocZ=? , CurDirX=? , CurDirZ=? , level=?, exp=?, MaxExpInThisLevel=?,WorldTable=?, WorldID=?, money=?, MoneyBank=?, reputation=?, MudosaPoint=?, SpPoint=?, BaseStr=?, LastStr=?, BaseCon=?, LastCon=?, BaseFoc=?, LastFoc=?, BaseDex=?, LastDex=?, BaseSol=?, LastSol=?, BaseEng=?, LastEng=?, BaseMaxLP=?, LastMaxLP=?, BaseMaxRp=?, LastMaxRP=? WHERE CharID = ?";
	char* save_query2 = "UPDATE characters SET BaseLpRegen=?, LastLpRegen=?, BaseLpSitdownRegen=?, LastLpSitdownRegen=?, BaseLpBattleRegen=?, LastLpBattleRegen=?, BaseEpRegen=?, LastEpRegen=?, BaseEpSitdownRegen=?, LastEpSitdownRegen=?, BaseEpBattleRegen=?, LastEpBattleRegen=?, BaseRpRegen=?, LastRpRegen=?, LastRpDimimutionRate=?, BasePhysicalOffence=?, LastPhysicalOffence=?, BasePhysicalDefence=? WHERE CharID = ?";
	char* save_query3 = "UPDATE characters SET LastPhysicalDefence=?, BaseEnergyOffence=?, LastEnergyOffence=?, BaseEnergyDefence=?, LastEnergyDefence=?, BaseAttackRate=?, LastAttackRate=?,  BaseDodgeRate=?, LastDodgeRate=?, BaseBlockRate=?, BaseBlockRate=?, LastBlockRate=?, BaseCurseSuccessRate=?, LastCurseSuccessRate=?, BaseCurseToleranceRate=?, LastCurseToleranceRate=?, BasePhysicalCriticalRate=?, LastPhysicalCriticalRate=? WHERE CharID = ?";
	char* save_query4 = "UPDATE characters SET BaseEnergyCriticalRate=?, LastEnergyCriticalRate=?, LastRunSpeed=?, BaseAttackSpeedRate=?, BaseAttackRange=?, LastAttackRange=?, CastingTimeChangePercent=?, CoolTimeChangePercent=?, KeepTimeChangePercent=?, DotValueChangePercent=?, DotTimeChangeAbsolute=?, RequiredEpChangePercent=?, HonestOffence=?, HonestDefence=?, StrangeOffence=?, StrangeDefence=?, WildOffence=?, WildDefence=? WHERE CharID = ?";
	char* save_query5 = "UPDATE characters SET EleganceOffence=?, EleganceDefence=?, FunnyDefence=?, FunnyOffence=?, FunnyDefence=?, ParalyzeToleranceRate=?, TerrorToleranceRate=?, ConfuseToleranceRate=?, StoneToleranceRate=?, CandyToleranceRate=?, ParalyzeKeepTimeDown=?, TerrorKeepTimeDown=?, ConfuseKeepTimeDown=?, StoneKeepTimeDown=?, CandyKeepTimeDown=?, BleedingKeepTimeDown=?, PoisonKeepTimeDown=?, StomachacheKeepTimeDown=?, CriticalBlockSuccessRate=?  WHERE CharID = ?";
	char* save_query6 = "UPDATE characters SET GuardRate=?, SkillDamageBlockModeSuccessRate=?, CurseBlockModeSuccessRate=?, KnockdownBlockModeSuccessRate=?, HtbBlockModeSuccessRate=?, SitDownLpRegenBonusRate=?, SitDownEpRegenBonusRate=?, PhysicalCriticalDamageBonusRate=?, EnergyCriticalDamageBonusRate=?, ItemUpgradeBonusRate=?, ItemUpgradeBreakBonusRate=?, CurLP=?, CurEP=?, CurRP=? WHERE CharID = ?";

	this->db->prepare(save_query);
	this->db->setFloat(1, this->vCurLoc.x);
	this->db->setFloat(2, this->vCurLoc.y);
	this->db->setFloat(3, this->vCurLoc.z);
	this->db->setFloat(4, this->vCurDir.x);
	this->db->setFloat(5, this->vCurDir.z);
	this->db->setInt(6, this->pcProfile->byLevel);
	this->db->setInt(7, this->pcProfile->dwCurExp);
	this->db->setInt(8, this->pcProfile->dwMaxExpInThisLevel);
	this->db->setInt(9, this->WorldTableID);
	this->db->setInt(10, this->WorldID);
	this->db->setInt(11, this->pcProfile->dwZenny);
	this->db->setInt(12, this->pcProfile->dwZenny);//bank
	this->db->setInt(13, this->pcProfile->dwReputation);
	this->db->setInt(14, this->pcProfile->dwMudosaPoint);
	this->db->setInt(15, this->pcProfile->dwSpPoint);
	this->db->setInt(16, this->pcProfile->avatarAttribute.byBaseStr);
	this->db->setInt(17, this->pcProfile->avatarAttribute.byLastStr);
	this->db->setInt(18, this->pcProfile->avatarAttribute.byBaseCon);
	this->db->setInt(19, this->pcProfile->avatarAttribute.byLastCon);
	this->db->setInt(20, this->pcProfile->avatarAttribute.byBaseFoc);
	this->db->setInt(21, this->pcProfile->avatarAttribute.byLastFoc);
	this->db->setInt(22, this->pcProfile->avatarAttribute.byBaseDex);
	this->db->setInt(23, this->pcProfile->avatarAttribute.byLastDex);
	this->db->setInt(24, this->pcProfile->avatarAttribute.byBaseSol);
	this->db->setInt(25, this->pcProfile->avatarAttribute.byLastSol);
	this->db->setInt(26, this->pcProfile->avatarAttribute.byBaseEng);
	this->db->setInt(27, this->pcProfile->avatarAttribute.byLastEng);
	this->db->setInt(28, this->pcProfile->avatarAttribute.wBaseMaxLP);
	this->db->setInt(29, this->pcProfile->avatarAttribute.wLastMaxLP);
	this->db->setInt(30, this->pcProfile->avatarAttribute.wBaseMaxRP);
	this->db->setInt(31, this->pcProfile->avatarAttribute.wLastMaxRP);
	this->db->setInt(32, this->pcProfile->charId);
	this->db->execute();

	// 1 DONE
	this->db->prepare(save_query2);
	this->db->setInt(1, this->pcProfile->avatarAttribute.wBaseLpRegen);
	this->db->setInt(2, this->pcProfile->avatarAttribute.wLastLpRegen);
	this->db->setInt(3, this->pcProfile->avatarAttribute.wBaseLpSitdownRegen);
	this->db->setInt(4, this->pcProfile->avatarAttribute.wLastLpSitdownRegen);
	this->db->setInt(5, this->pcProfile->avatarAttribute.wBaseLpBattleRegen);
	this->db->setInt(6, this->pcProfile->avatarAttribute.wLastLpBattleRegen);
	this->db->setInt(7, this->pcProfile->avatarAttribute.wBaseEpRegen);
	this->db->setInt(8, this->pcProfile->avatarAttribute.wLastEpRegen);
	this->db->setInt(9, this->pcProfile->avatarAttribute.wBaseEpSitdownRegen);
	this->db->setInt(10, this->pcProfile->avatarAttribute.wLastEpSitdownRegen);
	this->db->setInt(11, this->pcProfile->avatarAttribute.wBaseEpBattleRegen);
	this->db->setInt(12, this->pcProfile->avatarAttribute.wLastEpBattleRegen);
	this->db->setInt(13, this->pcProfile->avatarAttribute.wBaseRpRegen);
	this->db->setInt(14, this->pcProfile->avatarAttribute.wLastRpRegen);
	this->db->setInt(15, this->pcProfile->avatarAttribute.wLastRpDimimutionRate);
	this->db->setInt(16, this->pcProfile->avatarAttribute.wBasePhysicalOffence);
	this->db->setInt(17, this->pcProfile->avatarAttribute.wLastPhysicalOffence);
	this->db->setInt(18, this->pcProfile->avatarAttribute.wBasePhysicalDefence);
	this->db->setInt(19, this->pcProfile->charId);
	this->db->execute();

	// 2 DONE
	this->db->prepare(save_query3);
	this->db->setInt(1, this->pcProfile->avatarAttribute.wLastPhysicalDefence);
	this->db->setInt(2, this->pcProfile->avatarAttribute.wBaseEnergyOffence);
	this->db->setInt(3, this->pcProfile->avatarAttribute.wLastEnergyOffence);
	this->db->setInt(4, this->pcProfile->avatarAttribute.wBaseEnergyDefence);
	this->db->setInt(5, this->pcProfile->avatarAttribute.wLastEnergyDefence);
	this->db->setInt(6, this->pcProfile->avatarAttribute.wBaseAttackRate);
	this->db->setInt(7, this->pcProfile->avatarAttribute.wLastAttackRate);
	this->db->setInt(8, this->pcProfile->avatarAttribute.wBaseDodgeRate);
	this->db->setInt(9, this->pcProfile->avatarAttribute.wLastDodgeRate);
	this->db->setInt(10, this->pcProfile->avatarAttribute.wBaseBlockRate);
	this->db->setInt(11, this->pcProfile->avatarAttribute.wBaseBlockRate);
	this->db->setInt(12, this->pcProfile->avatarAttribute.wLastBlockRate);
	this->db->setInt(13, this->pcProfile->avatarAttribute.wBaseCurseSuccessRate);
	this->db->setInt(14, this->pcProfile->avatarAttribute.wLastCurseSuccessRate);
	this->db->setInt(15, this->pcProfile->avatarAttribute.wBaseCurseToleranceRate);
	this->db->setInt(16, this->pcProfile->avatarAttribute.wLastCurseToleranceRate);
	this->db->setInt(17, this->pcProfile->avatarAttribute.wBasePhysicalCriticalRate);
	this->db->setInt(18, this->pcProfile->avatarAttribute.wLastPhysicalCriticalRate);
	this->db->setInt(19, this->pcProfile->charId);
	this->db->execute();

	//3 DONE
	this->db->prepare(save_query4);
	this->db->setInt(1, this->pcProfile->avatarAttribute.wBaseEnergyCriticalRate);
	this->db->setInt(2, this->pcProfile->avatarAttribute.wLastEnergyCriticalRate);
	this->db->setFloat(3, this->pcProfile->avatarAttribute.fLastRunSpeed);
	this->db->setInt(4, this->pcProfile->avatarAttribute.wBaseAttackSpeedRate);
	this->db->setFloat(5, this->pcProfile->avatarAttribute.fBaseAttackRange);
	this->db->setFloat(6, this->pcProfile->avatarAttribute.fLastAttackRange);
	this->db->setFloat(7, this->pcProfile->avatarAttribute.fCastingTimeChangePercent);
	this->db->setFloat(8, this->pcProfile->avatarAttribute.fCoolTimeChangePercent);
	this->db->setFloat(9, this->pcProfile->avatarAttribute.fKeepTimeChangePercent);
	this->db->setFloat(10, this->pcProfile->avatarAttribute.fDotValueChangePercent);
	this->db->setFloat(11, this->pcProfile->avatarAttribute.fDotTimeChangeAbsolute);
	this->db->setFloat(12, this->pcProfile->avatarAttribute.fRequiredEpChangePercent);
	this->db->setFloat(13, this->pcProfile->avatarAttribute.fHonestOffence);
	this->db->setFloat(14, this->pcProfile->avatarAttribute.fHonestDefence);
	this->db->setFloat(15, this->pcProfile->avatarAttribute.fStrangeOffence);
	this->db->setFloat(16, this->pcProfile->avatarAttribute.fStrangeDefence);
	this->db->setFloat(17, this->pcProfile->avatarAttribute.fWildOffence);
	this->db->setFloat(18, this->pcProfile->avatarAttribute.fWildDefence);
	this->db->setInt(19, this->pcProfile->charId);
	this->db->execute();

	//4 DONE
	this->db->prepare(save_query5);
	this->db->setFloat(1, this->pcProfile->avatarAttribute.fEleganceOffence);
	this->db->setFloat(2, this->pcProfile->avatarAttribute.fEleganceDefence);
	this->db->setFloat(3, this->pcProfile->avatarAttribute.fFunnyDefence);
	this->db->setFloat(4, this->pcProfile->avatarAttribute.fFunnyOffence);
	this->db->setFloat(5, this->pcProfile->avatarAttribute.fFunnyDefence);
	this->db->setInt(6, this->pcProfile->avatarAttribute.wParalyzeToleranceRate);
	this->db->setInt(7, this->pcProfile->avatarAttribute.wTerrorToleranceRate);
	this->db->setInt(8, this->pcProfile->avatarAttribute.wConfuseToleranceRate);
	this->db->setInt(9, this->pcProfile->avatarAttribute.wStoneToleranceRate);
	this->db->setInt(10, this->pcProfile->avatarAttribute.wCandyToleranceRate);
	this->db->setFloat(11, this->pcProfile->avatarAttribute.fParalyzeKeepTimeDown);
	this->db->setFloat(12, this->pcProfile->avatarAttribute.fTerrorKeepTimeDown);
	this->db->setFloat(13, this->pcProfile->avatarAttribute.fConfuseKeepTimeDown);
	this->db->setFloat(14, this->pcProfile->avatarAttribute.fStoneKeepTimeDown);
	this->db->setFloat(15, this->pcProfile->avatarAttribute.fCandyKeepTimeDown);
	this->db->setFloat(16, this->pcProfile->avatarAttribute.fBleedingKeepTimeDown);
	this->db->setFloat(17, this->pcProfile->avatarAttribute.fPoisonKeepTimeDown);
	this->db->setFloat(18, this->pcProfile->avatarAttribute.fStomachacheKeepTimeDown);
	this->db->setFloat(19, this->pcProfile->avatarAttribute.fCriticalBlockSuccessRate);
	this->db->setInt(20, this->pcProfile->charId);
	this->db->execute();

	//5 DONE
	this->db->prepare(save_query6);
	this->db->setInt(1, this->pcProfile->avatarAttribute.wGuardRate);
	this->db->setFloat(2, this->pcProfile->avatarAttribute.fSkillDamageBlockModeSuccessRate);
	this->db->setFloat(3, this->pcProfile->avatarAttribute.fCurseBlockModeSuccessRate);
	this->db->setFloat(4, this->pcProfile->avatarAttribute.fKnockdownBlockModeSuccessRate);
	this->db->setFloat(5, this->pcProfile->avatarAttribute.fHtbBlockModeSuccessRate);
	this->db->setFloat(6, this->pcProfile->avatarAttribute.fSitDownLpRegenBonusRate);
	this->db->setFloat(7, this->pcProfile->avatarAttribute.fSitDownEpRegenBonusRate);
	this->db->setFloat(8, this->pcProfile->avatarAttribute.fPhysicalCriticalDamageBonusRate);
	this->db->setFloat(9, this->pcProfile->avatarAttribute.fEnergyCriticalDamageBonusRate);
	this->db->setFloat(10, this->pcProfile->avatarAttribute.fItemUpgradeBonusRate);
	this->db->setFloat(11, this->pcProfile->avatarAttribute.fItemUpgradeBreakBonusRate);
	this->db->setInt(12, this->pcProfile->wCurLP);
	this->db->setInt(13, this->pcProfile->wCurEP);
	this->db->setInt(14, this->pcProfile->wCurRP);
	this->db->setInt(15, this->pcProfile->charId);
	this->db->execute();

	delete db;
}

void		PlayerInfos::SaveMe()
{
	this->SavePlayerData();
	//this->SaveItems();
	//this->SaveSkills();
}
void		test(BYTE byAttributeTotalCount, void* pvRawData, sAVATAR_ATTRIBUTE* pAttributeData)
{
	BYTE* pbyCurrentPosition = (BYTE*)pvRawData;
	BYTE* pbyAttributeData = (BYTE*)pAttributeData;
	for (int i = 0; i <=byAttributeTotalCount; i++)
	{
		pbyCurrentPosition[i] = pbyAttributeData[i];
		//printf("pbyCurrentPosition[%d] = %d\n", i, pbyCurrentPosition[i]);
	}
}
void		PlayerInfos::setZero()
{
	this->pcProfile->avatarAttribute.wLastPhysicalOffence = this->pcProfile->avatarAttribute.wBasePhysicalOffence;
	this->pcProfile->avatarAttribute.wLastPhysicalDefence = this->pcProfile->avatarAttribute.wBasePhysicalDefence;
	this->pcProfile->avatarAttribute.wLastEnergyOffence = this->pcProfile->avatarAttribute.wBaseEnergyOffence;
	this->pcProfile->avatarAttribute.wLastEnergyDefence = this->pcProfile->avatarAttribute.wBaseEnergyDefence;
	this->pcProfile->avatarAttribute.wLastAttackSpeedRate = this->pcProfile->avatarAttribute.wBaseAttackSpeedRate;
	this->pcProfile->avatarAttribute.fLastAttackRange = this->pcProfile->avatarAttribute.fBaseAttackRange;

	this->pcProfile->avatarAttribute.byLastCon = this->pcProfile->avatarAttribute.byBaseCon;
	this->pcProfile->avatarAttribute.byLastDex = this->pcProfile->avatarAttribute.byBaseDex;
	this->pcProfile->avatarAttribute.byLastEng = this->pcProfile->avatarAttribute.byBaseEng;
	this->pcProfile->avatarAttribute.byLastFoc = this->pcProfile->avatarAttribute.byBaseFoc;
	this->pcProfile->avatarAttribute.byLastSol = this->pcProfile->avatarAttribute.byBaseSol;
	this->pcProfile->avatarAttribute.byLastStr = this->pcProfile->avatarAttribute.byBaseStr;

	this->pcProfile->avatarAttribute.wLastMaxRP = this->pcProfile->avatarAttribute.wLastMaxRP;
	this->pcProfile->avatarAttribute.wLastMaxEP = this->pcProfile->avatarAttribute.wBaseMaxEP;
	this->pcProfile->wCurLP = this->pcProfile->avatarAttribute.wLastMaxLP = this->pcProfile->avatarAttribute.wBaseMaxLP;

	this->pcProfile->avatarAttribute.wLastBlockRate = this->pcProfile->avatarAttribute.wBaseBlockRate;
	this->pcProfile->avatarAttribute.wLastAttackRate = this->pcProfile->avatarAttribute.wBaseAttackRate;
	this->pcProfile->avatarAttribute.wLastDodgeRate = this->pcProfile->avatarAttribute.wBaseDodgeRate;

	this->pcProfile->avatarAttribute.wLastEnergyCriticalRate = 1;
	this->pcProfile->avatarAttribute.fEnergyCriticalDamageBonusRate = 1;
	this->pcProfile->avatarAttribute.fPhysicalCriticalDamageBonusRate = 1;

	this->pcProfile->avatarAttribute.wLastEnergyCriticalRate = this->pcProfile->avatarAttribute.wBaseEnergyCriticalRate;
	this->pcProfile->avatarAttribute.wLastPhysicalCriticalRate = this->pcProfile->avatarAttribute.wBasePhysicalCriticalRate;
}

void		PlayerInfos::calculeMyStat(CGameServer * app)
{
	app->db->prepare("SELECT * FROM items WHERE owner_ID = ? AND place=7");
	app->db->setInt(1, this->pcProfile->charId);
	app->db->execute();
	this->setZero();
	CItemTable *itemTbl = app->g_pTableContainer->GetItemTable();
	CItemOptionTable* itemOptionTbl = app->g_pTableContainer->GetItemOptionTable();
	while (app->db->fetch())
	{
		sITEM_TBLDAT* pItemData = reinterpret_cast<sITEM_TBLDAT*>(itemTbl->FindData(app->db->getInt("tblidx")));
		sITEM_DATA* pDataItem = (sITEM_DATA*)pItemData->tblidx;
		sITEM_DATA* pDataItem23 = (sITEM_DATA*)pItemData->Name;
		if (pItemData->wPhysical_Offence < 65535 && pItemData->wPhysical_Offence > 0)
		{
			this->pcProfile->avatarAttribute.wLastPhysicalOffence += pItemData->wPhysical_Offence;
			UpdateAttribute(this->GetAvatarandle(), ATTRIBUTE_TO_UPDATE_PHYSICAL_OFFENCE_LAST, this->pcProfile->avatarAttribute.wLastPhysicalOffence);
		}
		if (pItemData->wPhysical_Defence < 65535 && pItemData->wPhysical_Defence > 0)
		{
			this->pcProfile->avatarAttribute.wLastPhysicalDefence += pItemData->wPhysical_Defence;
			UpdateAttribute(this->GetAvatarandle(), ATTRIBUTE_TO_UPDATE_PHYSICAL_DEFENCE_LAST, this->pcProfile->avatarAttribute.wLastPhysicalDefence);
		}
		if (pItemData->wEnergy_Offence < 65535 && pItemData->wEnergy_Offence > 0)
		{
			this->pcProfile->avatarAttribute.wLastEnergyOffence += pItemData->wEnergy_Offence;
			UpdateAttribute(this->GetAvatarandle(), ATTRIBUTE_TO_UPDATE_ENERGY_OFFENCE_LAST, this->pcProfile->avatarAttribute.wLastEnergyOffence);
		}
		if (pItemData->wEnergy_Defence < 65535 && pItemData->wEnergy_Defence > 0)
		{
			this->pcProfile->avatarAttribute.wLastEnergyDefence += pItemData->wEnergy_Defence;
			UpdateAttribute(this->GetAvatarandle(), ATTRIBUTE_TO_UPDATE_ENERGY_DEFENCE_LAST, this->pcProfile->avatarAttribute.wLastEnergyDefence);
		}
		if (pItemData->wAttack_Speed_Rate < 65535 && pItemData->wAttack_Speed_Rate > 0)
		{	
			this->pcProfile->avatarAttribute.wLastAttackSpeedRate += pItemData->wAttack_Speed_Rate;
			UpdateAttribute(this->GetAvatarandle(), ATTRIBUTE_TO_UPDATE_ATTACK_RATE_LAST,  this->pcProfile->avatarAttribute.wLastAttackSpeedRate);
		}
		if (pItemData->fAttack_Range_Bonus < 65535 && pItemData->fAttack_Range_Bonus > 0)
		{
			this->pcProfile->avatarAttribute.fLastAttackRange += pItemData->fAttack_Range_Bonus;
			UpdateAttribute(this->GetAvatarandle(), ATTRIBUTE_TO_UPDATE_ATTACK_RANGE_LAST, (RwUInt32)this->pcProfile->avatarAttribute.fLastAttackRange);
		}		
		pItemData->dwPhysical_OffenceUpgrade;
		pItemData->dwPhysical_DefenceUpgrade;
		pItemData->dwEnergy_OffenceUpgrade;
		pItemData->dwEnergy_DefenceUpgrade;
		pItemData->byNeed_Str;
		pItemData->byNeed_Sol;
		pItemData->byNeed_Foc;
		pItemData->byNeed_Eng;
		pItemData->byNeed_Con;
		pItemData->byNeed_Dex;
		pItemData->byBattle_Attribute;
	}
	app->db->prepare("UPDATE characters SET LastAttackSpeedRate = ?, LastEnergyDefence = ?, LastEnergyOffence = ?,LastPhysicalDefence = ?, LastPhysicalOffence = ? WHERE CharID = ?");
	app->db->setInt(1, this->pcProfile->avatarAttribute.wLastAttackSpeedRate);
	app->db->setInt(2, this->pcProfile->avatarAttribute.wLastEnergyDefence);
	app->db->setInt(3,this->pcProfile->avatarAttribute.wLastEnergyOffence);
	app->db->setInt(4, this->pcProfile->avatarAttribute.wLastPhysicalDefence);
	app->db->setInt(5, this->pcProfile->avatarAttribute.wLastPhysicalOffence);
	app->db->setInt(6,  this->pcProfile->charId);
	app->db->execute();

	app->db->prepare("UPDATE characters SET LastStr = ?, LastCon = ?, LastFoc = ?, LastDex = ?,LastSol = ?, LastEng = ? WHERE CharID = ?");
	app->db->setInt(1, this->pcProfile->avatarAttribute.byLastStr);
	app->db->setInt(2, this->pcProfile->avatarAttribute.byLastCon);
	app->db->setInt(3, this->pcProfile->avatarAttribute.byLastFoc);
	app->db->setInt(4, this->pcProfile->avatarAttribute.byLastDex);
	app->db->setInt(5, this->pcProfile->avatarAttribute.byLastSol);
	app->db->setInt(6, this->pcProfile->avatarAttribute.byLastEng);
	app->db->setInt(7,  this->pcProfile->charId);
	app->db->execute();

	app->db->prepare("UPDATE characters SET LastMaxLP = ?, LastMaxEP = ?, LastMaxRP = ?, LastPhysicalCriticalRate = ?, LastEnergyCriticalRate = ? WHERE CharID = ?");
	app->db->setInt(1, this->pcProfile->avatarAttribute.wLastMaxLP);
	app->db->setInt(2, this->pcProfile->avatarAttribute.wLastMaxEP);
	app->db->setInt(3, this->pcProfile->avatarAttribute.wLastMaxRP);
	app->db->setInt(4, this->pcProfile->avatarAttribute.wLastPhysicalCriticalRate);
	app->db->setInt(5, this->pcProfile->avatarAttribute.wLastEnergyCriticalRate);
	app->db->setInt(6,  this->pcProfile->charId);
	app->db->execute();

	app->db->prepare("SELECT * FROM characters WHERE CharID = ?");
	app->db->setInt(1, this->pcProfile->charId);
	app->db->execute();
	app->db->fetch();
}


void PlayerInfos::UpdateAttribute(RwUInt32 Handle, RwUInt32 Attribute, RwUInt32 Amount)
{
        CNtlPacket packet(sizeof(sGU_AVATAR_ATTRIBUTE_UPDATE));
        sGU_AVATAR_ATTRIBUTE_UPDATE * res = (sGU_AVATAR_ATTRIBUTE_UPDATE *)packet.GetPacketData();	
		
		sAVATAR_ATTRIBUTE_LINK avtLink = CNtlAvatar::GetInstance()->ConvertAttributeToAttributeLink(&this->pcProfile->avatarAttribute);
		DWORD buffer[1024];//Thanks Daneos
		DWORD datasize;

		CNtlBitFlagManager changedFlag;
		changedFlag.Create(&this->pcProfile->avatarAttribute,ATTRIBUTE_TO_UPDATE_COUNT);
		
		for (BYTE byIndex = ATTRIBUTE_TO_UPDATE_FIRST; byIndex <= ATTRIBUTE_TO_UPDATE_LAST; byIndex++)
		{
			changedFlag.Set(byIndex);
		}
		
		if (CNtlAvatar::GetInstance()->SaveAvatarAttribute(&changedFlag, &avtLink, &buffer, &datasize) == false)
		{
			printf("ERROR IN UPDATE ATTRIBUTE");
		}

		memcpy(res->abyFlexibleField, &buffer, ((UCHAR_MAX - 1) / 8 + 1) + sizeof(sAVATAR_ATTRIBUTE));

		res->byAttributeTotalCount = 101;
		res->hHandle = Handle;
		res->wOpCode = GU_AVATAR_ATTRIBUTE_UPDATE;		

        packet.SetPacketLen(sizeof(sGU_AVATAR_ATTRIBUTE_UPDATE));
		g_pApp->Send(this->MySession, &packet);
}

DWORD PlayerInfos::GetMoney()
{
	return this->pcProfile->dwZenny;
}

int PlayerInfos::GetBankMoney()
{
	app->db->prepare("SELECT MoneyBank FROM characters WHERE CharID = ?");
	app->db->setInt(1, this->pcProfile->charId);
	app->db->execute();
	app->db->fetch();
	int bankmoney = app->db->getInt("MoneyBank");
	return bankmoney;
}


void PlayerInfos::SetBankMoney(int Amount)
{
	app->db->prepare("UPDATE characters SET MoneyBank = ? WHERE CharID = ?");
	app->db->setInt(1, Amount);
	app->db->setInt(2, this->pcProfile->charId);
	app->db->execute();
}

void PlayerInfos::SetMoney(int Amount)
{
	DWORD currentMoney = this->GetMoney();

	if ((int)currentMoney <= Amount)
	{
		currentMoney = 0;
	}
	else
	{
		currentMoney = Amount;
	}

}


