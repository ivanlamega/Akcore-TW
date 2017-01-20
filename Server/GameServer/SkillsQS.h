#include "stdafx.h"
#include "Vector.h"
#include <list>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "Character.h"

#ifndef PLAYERS_SKILLS_H
#define PLAYERS_SKILLS_H
class PlayersSkills{
	///-------------Constructor & Destructor-------------///
	public:
		PlayersSkills(int CharID);
		~PlayersSkills();
   ///-------------------------------------///
	private:
		int CharID;
		BYTE SkillCounter;
		BYTE SkillBuffCounter;
		BYTE HTBSkillCounter;
		BYTE QuickSlotCounter;		
		sSKILL_INFO		aSkillInfo[NTL_MAX_PC_HAVE_SKILL];
		sBUFF_INFO		aBuffInfo[NTL_MAX_BLESS_BUFF_CHARACTER_HAS + NTL_MAX_CURSE_BUFF_CHARACTER_HAS];
		sHTB_SKILL_INFO	aHTBSkillnfo[NTL_HTB_MAX_PC_HAVE_HTB_SKILL];
		sQUICK_SLOT_PROFILE	asQuickSlotData[NTL_CHAR_QUICK_SLOT_MAX_COUNT];
		
	public:
		sSKILL_INFO* GetSkills();
		sBUFF_INFO* GetSkillBuff();
		sHTB_SKILL_INFO* GetHTBSkills();
		sQUICK_SLOT_PROFILE* GetQuickSlot();
		BYTE GetSkillCount();
		BYTE GetSkillBuffCount();
		BYTE GetHTBSkillCount();
		BYTE GetQuickSlotCount();
		void LoadSkills();
		void LoadSkillBuff();
		void LoadHTB();
		void LoadQuickSlot();		
};

#endif