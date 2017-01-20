#include "stdafx.h"
#include "Vector.h"
#include <list>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "Avatar.h"

#ifndef PLAYERS_ATTRIBUTES_H
#define PLAYERS_ATTRIBUTES_H   

class PlayerAttributes{
	///-------------Constructor & Destructor-------------///
	public:
		PlayerAttributes(HSESSION PlayerSession);
		~PlayerAttributes();
	///-------------------------------------///
	private:
		HSESSION PlayerSession;
		sAVATAR_ATTRIBUTE sPlayerAttribute;
		sAVATAR_ATTRIBUTE_LINK sPlayerAttributeLink;
	public:
		void LoadAttributesFromDB(int charID);
		void UpdateStatsUsingScouterChips(RwUInt32 Handle, TBLIDX OptionTblidx = INVALID_TBLIDX, bool bDecrease = false);
		void UpdateAvatarAttributes(RwUInt32 Handle);
		sAVATAR_ATTRIBUTE GetAvatarAttribute();
		sAVATAR_ATTRIBUTE_LINK GetAvatarAttributeLink();
};

#endif