#include "stdafx.h"
#include "ChatServer.h"


//--------------------------------------------------------------------------------------//
//		Enter Chat Server
//--------------------------------------------------------------------------------------//
void CClientSession::SendEnterChat(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_ENTER_CHAT * req = (sUT_ENTER_CHAT *)pPacket->GetPacketData();

	this->accountID = req->accountId;

	app->db->prepare("SELECT * FROM characters WHERE AccountID = ? AND isOnline = 1");
	app->db->setInt(1, req->accountId);
	app->db->execute();
	while (app->db->fetch())
	{
		this->characterID = app->db->getInt("CharID");
		this->charName = app->db->getString("CharName");
		this->avatarHandle = app->db->getInt("OnlineID");
		this->guildName = app->db->getInt("GuildName");
		if (app->FindUser(app->db->getString("CharName").c_str()))
			continue;
		else
			break;
	}

	
	printf("ok\n");
	CNtlPacket packet(sizeof(sTU_ENTER_CHAT_RES));
	sTU_ENTER_CHAT_RES * res = (sTU_ENTER_CHAT_RES *)packet.GetPacketData();

	res->wOpCode = TU_ENTER_CHAT_RES;
	res->wResultCode = CHAT_SUCCESS;
	app->AddUser(app->db->getString("CharName").c_str(), this);
	printf("unique user id: %i AND getuser: %s ", this->GetavatarHandle(), this->charName.c_str() );

	packet.SetPacketLen( sizeof(sTU_ENTER_CHAT_RES) );
	int rc = g_pApp->Send( this->GetHandle(), &packet );

	//Load Guild Information
	this->SendLoadGuildInfo(pPacket, app);

}

//--------------------------------------------------------------------------------------//
//		Say Chat
//--------------------------------------------------------------------------------------//
void CClientSession::SendSayReq(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_CHAT_MESSAGE_SAY * req = (sUT_CHAT_MESSAGE_SAY *)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sTU_CHAT_MESSAGE_SAY));
	sTU_CHAT_MESSAGE_SAY * res = (sTU_CHAT_MESSAGE_SAY *)packet.GetPacketData();

	res->wOpCode = TU_CHAT_MESSAGE_SAY;
	
	wcscpy_s(res->awchMessage, NTL_MAX_LENGTH_OF_CHAT_MESSAGE_UNICODE, req->awchMessage );
	res->wMessageLengthInUnicode = req->wMessageLengthInUnicode;
	res->hSubject = GetavatarHandle();

	wcscpy_s(res->awchSenderCharName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(this->charName).c_str() );

	packet.SetPacketLen( sizeof(sTU_CHAT_MESSAGE_SAY) );
	int rc = g_pApp->Send( this->GetHandle(), &packet );
	app->UserBroadcastothers(&packet, this);
	
}

//--------------------------------------------------------------------------------------//
//		Shout Chat
//--------------------------------------------------------------------------------------//
void CClientSession::SendShoutReq(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_CHAT_MESSAGE_SHOUT * req = (sUT_CHAT_MESSAGE_SHOUT *)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sTU_CHAT_MESSAGE_SHOUT));
	sTU_CHAT_MESSAGE_SHOUT * res = (sTU_CHAT_MESSAGE_SHOUT *)packet.GetPacketData();

	res->wOpCode = TU_CHAT_MESSAGE_SHOUT;

	wcscpy_s(res->awchMessage, NTL_MAX_LENGTH_OF_CHAT_MESSAGE_UNICODE, req->awchMessage);
	res->wMessageLengthInUnicode = req->wMessageLengthInUnicode;
	res->hSubject = GetavatarHandle();

	wcscpy_s(res->awchSenderCharName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(this->charName).c_str());

	packet.SetPacketLen(sizeof(sTU_CHAT_MESSAGE_SHOUT));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
	app->UserBroadcastothers(&packet, this);

}
/**
* Send Find Party - Luiz45
*/
void CClientSession::SendFindParty(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_CHAT_MESSAGE_FIND_PARTY* req = (sUT_CHAT_MESSAGE_FIND_PARTY*)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sTU_CHAT_MESSAGE_FIND_PARTY));
	sTU_CHAT_MESSAGE_FIND_PARTY* res = (sTU_CHAT_MESSAGE_FIND_PARTY*)packet.GetPacketData();

	wcscpy_s(res->awchSenderCharName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(this->charName).c_str());
	wcscpy_s(res->awchMessage, NTL_MAX_LENGTH_OF_CHAT_MESSAGE_UNICODE, req->awchMessage);
	res->wMessageLengthInUnicode = req->wMessageLengthInUnicode;
	res->byChannelID = 0;
	res->hSubject = this->GetavatarHandle();
	res->wOpCode = TU_CHAT_MESSAGE_FIND_PARTY;

	packet.SetPacketLen(sizeof(sTU_CHAT_MESSAGE_FIND_PARTY));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
	app->UserBroadcastothers(&packet, this);
}
/*
* Message Trade - Luiz45
*/
void CClientSession::SendMessageTrade(CNtlPacket* pPacket, CChatServer* app)
{
	sUT_CHAT_MESSAGE_TRADE* req = (sUT_CHAT_MESSAGE_TRADE*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sTU_CHAT_MESSAGE_TRADE));
	sTU_CHAT_MESSAGE_TRADE* res = (sTU_CHAT_MESSAGE_TRADE*)packet.GetPacketData();

	wcscpy_s(res->awchSenderCharName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(this->charName).c_str());
	wcscpy_s(res->awchMessage, NTL_MAX_LENGTH_OF_CHAT_MESSAGE_UNICODE, req->awchMessage);
	res->wMessageLengthInUnicode = req->wMessageLengthInUnicode;
	res->byChannelID = 0;
	res->hSubject = this->GetavatarHandle();
	res->wOpCode = TU_CHAT_MESSAGE_TRADE;

	packet.SetPacketLen(sizeof(sTU_CHAT_MESSAGE_TRADE));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
	app->UserBroadcastothers(&packet, this);
}
//--------------------------------------------------------------------------------------//
//		Whisper Chat
//--------------------------------------------------------------------------------------//
void CClientSession::SendWhisperReq(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_CHAT_MESSAGE_WHISPER * req = (sUT_CHAT_MESSAGE_WHISPER *)pPacket->GetPacketData();
	CNtlPacket packet(sizeof(sTU_CHAT_MESSAGE_WHISPER));
	sTU_CHAT_MESSAGE_WHISPER * res = (sTU_CHAT_MESSAGE_WHISPER *)packet.GetPacketData();

	char chReceiverCharName[17];
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, req->awchReceiverCharName, -1, chReceiverCharName, 17, &DefChar, NULL);

	std::string ss(chReceiverCharName);
	
	app->db->prepare("SELECT * from Characters where CharName = ?");
	app->db->setString(1, ss);
	app->db->execute();
	app->db->fetch();
	
	CClientSession* recevierHandle = app->FindUserSession(chReceiverCharName);

	res->wOpCode = TU_CHAT_MESSAGE_WHISPER;
	wcscpy_s(res->awchMessage, NTL_MAX_LENGTH_OF_CHAT_MESSAGE_UNICODE, req->awchMessage);
	res->wMessageLengthInUnicode = req->wMessageLengthInUnicode;
	wcscpy_s(res->awchSenderCharName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(this->charName).c_str());
	if (app->db->getInt("IsOnline") == 1)
	{
		packet.SetPacketLen(sizeof(sTU_CHAT_MESSAGE_WHISPER));
		int rc = g_pApp->Send(this->GetHandle(), &packet); 
		g_pApp->SendTo(recevierHandle, &packet);
	}
	else
	{
		//Error Messages Handler for Client
	}

}



//--------------------------------------------------------------------------------------//
//		Load Guild info
//--------------------------------------------------------------------------------------//
void CClientSession::SendLoadGuildInfo(CNtlPacket * pPacket, CChatServer * app)
{

	app->db->prepare("CALL GuildInfo (?, @guild_id,@guild_name,@guild_master,@guild_notice,@guild_rep,@second_guild_master, @guild_functions)");
	app->db->setInt(1, this->GetCharacterId());
	app->db->execute();

	app->db->execute("SELECT @guild_id,@guild_name,@guild_master,@guild_notice,@guild_rep,@second_guild_master,@guild_functions");
	app->db->fetch();

//Check if user inside guild
	if(app->db->getInt("@guild_id") != 0)
	{
	//SET GUILD INFO
		CNtlPacket packet(sizeof(sTU_GUILD_INFO));
		sTU_GUILD_INFO * res = (sTU_GUILD_INFO *)packet.GetPacketData();

		int guildID = app->db->getInt("@guild_id");
		this->guildName = app->db->getString("@guild_name");

		res->wOpCode = TU_GUILD_INFO;
		res->guildInfo.dwGuildReputation = app->db->getInt("@guild_rep");
		res->guildInfo.guildId = guildID;
		res->guildInfo.guildMaster = app->db->getInt("@guild_master");
		res->guildInfo.guildSecondMaster[0] = app->db->getInt("@second_guild_master");
		res->guildInfo.sMark.byMarkInColor = 5;
		res->guildInfo.sMark.byMarkInLine = 4;
		res->guildInfo.sMark.byMarkMain = 1;
		res->guildInfo.sMark.byMarkMainColor =3;
		res->guildInfo.sMark.byMarkOutColor = 2;
		res->guildInfo.sMark.byMarkOutLine = 1;
		//res->guildInfo.sMark.IsIntialized = 1;
		wcscpy_s(res->guildInfo.wszName, NTL_MAX_SIZE_GUILD_NAME_IN_UNICODE, s2ws(app->db->getString("@guild_name")).c_str());
		wcscpy_s(res->guildInfo.awchName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(this->GetCharName()).c_str());
		wcscpy_s(res->guildInfo.awchNotice, NTL_MAX_LENGTH_OF_GUILD_NOTICE_UNICODE, s2ws(app->db->getString("@guild_notice")).c_str());
		res->guildInfo.qwGuildFunctionFlag = app->db->getInt("@guild_functions");
		packet.SetPacketLen( sizeof(sTU_GUILD_INFO));
		int rc = g_pApp->Send( this->GetHandle(), &packet);
		printf("Guild ID: %i \n",guildID);

	//SET GUILD MEMBERS
		CNtlPacket packet2(sizeof(sTU_GUILD_MEMBER_INFO));
		sTU_GUILD_MEMBER_INFO * res2 = (sTU_GUILD_MEMBER_INFO *)packet2.GetPacketData();

		app->db->prepare("SELECT CharID,CharName,Level,Race,Class FROM characters WHERE GuildID = ?");
		app->db->setInt(1, guildID);
		app->db->execute();

		while(app->db->fetch())
		{
			res2->wOpCode = TU_GUILD_MEMBER_INFO;
			res2->guildMemberInfo.bIsOnline = true;
			res2->guildMemberInfo.byClass = app->db->getInt("Class");
			res2->guildMemberInfo.byLevel = app->db->getInt("Level");
			res2->guildMemberInfo.byRace = app->db->getInt("Race");
			res2->guildMemberInfo.charId = app->db->getInt("CharID");
			res2->guildMemberInfo.dwReputation = 0;
			wcscpy_s(res2->guildMemberInfo.wszMemberName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(app->db->getString("CharName")).c_str());

			packet2.SetPacketLen( sizeof(sTU_GUILD_MEMBER_INFO));
			rc = g_pApp->Send( this->GetHandle(), &packet2);
		}
	}

}
//--------------------------------------------------------------------------------------//
//		Disband Guild
//--------------------------------------------------------------------------------------//
void CClientSession::SendDisbandGuild(CNtlPacket * pPacket, CChatServer * app)
{
	app->db->prepare("CALL DisbandGuild (?,?, @wResultCode)");
	app->db->setString(1, this->GetGuildName().c_str());
	app->db->setInt(2, this->GetCharacterId());
	app->db->execute();

	app->db->execute("SELECT @wResultCode");
	app->db->fetch();

	CNtlPacket packet(sizeof(sTU_GUILD_DISBAND_RES));
	sTU_GUILD_DISBAND_RES * res = (sTU_GUILD_DISBAND_RES *)packet.GetPacketData();

	res->wOpCode = TU_GUILD_DISBAND_RES;
	res->wResultCode = app->db->getInt("@wResultCode");

	packet.SetPacketLen( sizeof(sTU_GUILD_DISBAND_RES));
	int rc = g_pApp->Send( this->GetHandle(), &packet);

	if(app->db->getInt("@wResultCode") == 320)
	{
		CNtlPacket packet2(sizeof(sTU_GUILD_DISBANDED_NFY));
		sTU_GUILD_DISBANDED_NFY * res2 = (sTU_GUILD_DISBANDED_NFY *)packet2.GetPacketData();
		res2->wOpCode = TU_GUILD_DISBANDED_NFY;
		packet2.SetPacketLen( sizeof(sTU_GUILD_DISBANDED_NFY));
		//app->UserBroadcastothers(&packet2, this); // CHANGE THAT IT ONLY BROADCAST TO OTHER GUILD MEMBERS
		rc = g_pApp->Send( this->GetHandle(), &packet2);
	}

}
//--------------------------------------------------------------------------------------//
//		Load guild member info
//--------------------------------------------------------------------------------------//
void CClientSession::SendLoadGuildMember(CNtlPacket * pPacket, CChatServer * app, int GuildID)
{
	CNtlPacket packet(sizeof(TU_GUILD_MEMBER_INFO));
	 
	sTU_GUILD_MEMBER_INFO * res = (sTU_GUILD_MEMBER_INFO *)packet.GetPacketData();
	app->db->prepare("SELECT * from guild_members where GuildID = ?");
	app->db->setInt(1, GuildID);
	app->db->execute();

	while (app->db->fetch())
	{

		app->db2->prepare("SELECT * from characters where CharName = ?");
		app->db2->setString(1, app->db->getString("MemberName"));
		app->db2->execute();
		while (app->db2->fetch())
		{
			res->guildMemberInfo.bIsOnline = app->db2->getBoolean("IsOnline");
			res->guildMemberInfo.byClass = app->db2->getInt("Class");
			res->guildMemberInfo.byLevel = app->db2->getInt("Level");
			res->guildMemberInfo.byRace = app->db2->getInt("Race");
			res->guildMemberInfo.charId = app->db2->getInt("CharID");
			res->guildMemberInfo.dwReputation = app->db->getInt("MemberReputation");	
			wcscpy_s(res->guildMemberInfo.wszMemberName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(app->db2->getString("CharName")).c_str());		
			
			packet.SetPacketLen(sizeof(sTU_GUILD_MEMBER_INFO));
			int rc = g_pApp->Send(this->GetHandle(), &packet);
		}

	}
	
}
//--------------------------------------------------------------------------------------//
//		Change guild notice
//--------------------------------------------------------------------------------------//
void CClientSession::SendGuildChangeNotice(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_GUILD_CHANGE_NOTICE_REQ * req = (sUT_GUILD_CHANGE_NOTICE_REQ *)pPacket->GetPacketData();

	app->db->prepare("CALL UpdateGuildNotice (?,?, @wResultCode)");
	app->db->setInt(1, this->GetCharacterId());
	app->db->setString(2, Ntl_WC2MB(req->awchNotice));
	app->db->execute();

	app->db->execute("SELECT @wResultCode");
	app->db->fetch();

	CNtlPacket packet(sizeof(sTU_GUILD_CHANGE_NOTICE_RES));
	sTU_GUILD_CHANGE_NOTICE_RES * res = (sTU_GUILD_CHANGE_NOTICE_RES *)packet.GetPacketData();
	res->wOpCode = TU_GUILD_CHANGE_NOTICE_RES;
	res->wResultCode = app->db->getInt("@wResultCode");
	packet.SetPacketLen( sizeof(sTU_GUILD_CHANGE_NOTICE_RES));
	int rc = g_pApp->Send( this->GetHandle(), &packet);

	if(app->db->getInt("@wResultCode") == 300)
	{
		CNtlPacket packet2(sizeof(sTU_GUILD_CHANGE_NOTICE_NFY));
		sTU_GUILD_CHANGE_NOTICE_NFY * res2 = (sTU_GUILD_CHANGE_NOTICE_NFY *)packet2.GetPacketData();
		res2->wOpCode = TU_GUILD_CHANGE_NOTICE_NFY;
		wcscpy_s(res2->awchCharName, NTL_MAX_SIZE_CHAR_NAME_UNICODE, s2ws(this->GetCharName()).c_str());
		memcpy(res2->awchNotice, req->awchNotice, sizeof(wchar_t)* NTL_MAX_LENGTH_OF_GUILD_NOTICE_UNICODE);
		res2->wNoticeLengthInUnicode = req->wNoticeLengthInUnicode;
		//app->UserBroadcastothers(&packet2, this); // CHANGE THAT IT ONLY BROADCAST TO OTHER GUILD MEMBERS
		packet2.SetPacketLen( sizeof(sTU_GUILD_CHANGE_NOTICE_NFY));
		rc = g_pApp->Send( this->GetHandle(), &packet2);
	}
	
}

//--------------------------------------------------------------------------------------//
//		Leave Guild
//--------------------------------------------------------------------------------------//
void CClientSession::SendLeaveGuildReq(CNtlPacket * pPacket, CChatServer * app)
{
	app->db->prepare("CALL LeaveGuild (?, @wResultCode)");
	app->db->setInt(1, this->GetCharacterId());
	app->db->execute();

	app->db->execute("SELECT @wResultCode");
	app->db->fetch();

	CNtlPacket packet(sizeof(sTU_GUILD_LEAVE_RES));
	sTU_GUILD_LEAVE_RES * res = (sTU_GUILD_LEAVE_RES *)packet.GetPacketData();
	res->wOpCode = TU_GUILD_LEAVE_RES;
	res->wResultCode = app->db->getInt("@wResultCode");

	packet.SetPacketLen( sizeof(sTU_GUILD_LEAVE_RES));
	int rc = g_pApp->Send( this->GetHandle(), &packet);

	printf("%i \n ", app->db->getInt("@wResultCode"));

	if(app->db->getInt("@wResultCode") == 348)
	{
		CNtlPacket packet2(sizeof(sTU_GUILD_MEMBER_LEFT_NFY));
		sTU_GUILD_MEMBER_LEFT_NFY * res2 = (sTU_GUILD_MEMBER_LEFT_NFY *)packet2.GetPacketData();

		res2->wOpCode = TU_GUILD_MEMBER_LEFT_NFY;
		res2->memberCharId = this->GetCharacterId();
		//app->UserBroadcastothers(&packet2, this); // CHANGE THAT IT ONLY BROADCAST TO OTHER GUILD MEMBERS
		packet2.SetPacketLen( sizeof(sTU_GUILD_MEMBER_LEFT_NFY));
		rc = g_pApp->Send( this->GetHandle(), &packet2);
	}

}
//--------------------------------------------------------------------------------------//
//		Kick from Guild
//--------------------------------------------------------------------------------------//
void CClientSession::SendKickFromGuildReq(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_GUILD_KICK_OUT_REQ * req = (sUT_GUILD_KICK_OUT_REQ *)pPacket->GetPacketData();
	
	app->db->prepare("CALL KickGuild (?, @wResultCode)");
	app->db->setInt(1, req->targetMemberCharId);
	app->db->execute();

	app->db->execute("SELECT @wResultCode");
	app->db->fetch();

	printf("target id %i \n", req->targetMemberCharId);

	CNtlPacket packet(sizeof(sTU_GUILD_KICK_OUT_RES));
	sTU_GUILD_KICK_OUT_RES * res = (sTU_GUILD_KICK_OUT_RES *)packet.GetPacketData();
	res->wOpCode = TU_GUILD_KICK_OUT_RES;
	res->wResultCode = app->db->getInt("@wResultCode");

	packet.SetPacketLen( sizeof(sTU_GUILD_KICK_OUT_RES));
	int rc = g_pApp->Send( this->GetHandle(), &packet);

	printf("%i \n ", app->db->getInt("@wResultCode"));

	if(app->db->getInt("@wResultCode") == CHAT_SUCCESS)
	{
		CNtlPacket packet2(sizeof(sTU_GUILD_MEMBER_KICKED_OUT_NFY));
		sTU_GUILD_MEMBER_KICKED_OUT_NFY * res2 = (sTU_GUILD_MEMBER_KICKED_OUT_NFY *)packet2.GetPacketData();

		res2->wOpCode = TU_GUILD_MEMBER_KICKED_OUT_NFY;
		res2->memberCharId = req->targetMemberCharId;
		//app->UserBroadcastothers(&packet2, this); // CHANGE THAT IT ONLY BROADCAST TO OTHER GUILD MEMBERS
		packet2.SetPacketLen( sizeof(sTU_GUILD_MEMBER_KICKED_OUT_NFY));
		rc = g_pApp->Send( this->GetHandle(), &packet2);
		printf("kicked out %i \n", req->targetMemberCharId);
	}

}
//--------------------------------------------------------------------------------------//
//		Appoint second guildmaster
//--------------------------------------------------------------------------------------//
void CClientSession::SendNewSecondGuildMaster(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_GUILD_APPOINT_SECOND_MASTER_REQ * req = (sUT_GUILD_APPOINT_SECOND_MASTER_REQ *)pPacket->GetPacketData();

	app->db->prepare("CALL NewSecondGuildMaster (?,?, @wResultCode)");
	app->db->setInt(1, req->targetMemberCharId);
	app->db->setString(2, this->GetGuildName().c_str());
	app->db->execute();

	app->db->execute("SELECT @wResultCode");
	app->db->fetch();

	CNtlPacket packet(sizeof(sTU_GUILD_APPOINT_SECOND_MASTER_RES));
	sTU_GUILD_APPOINT_SECOND_MASTER_RES * res = (sTU_GUILD_APPOINT_SECOND_MASTER_RES *)packet.GetPacketData();

	res->wOpCode = TU_GUILD_APPOINT_SECOND_MASTER_RES;
	res->secondMasterCharId = req->targetMemberCharId;
	res->wResultCode = app->db->getInt("@wResultCode");

	packet.SetPacketLen( sizeof(sTU_GUILD_APPOINT_SECOND_MASTER_RES));
	int rc = g_pApp->Send( this->GetHandle(), &packet);


	if(app->db->getInt("@wResultCode") == CHAT_SUCCESS)
	{
		CNtlPacket packet2(sizeof(sTU_GUILD_SECOND_MASTER_APPOINTED_NFY));
		sTU_GUILD_SECOND_MASTER_APPOINTED_NFY * res2 = (sTU_GUILD_SECOND_MASTER_APPOINTED_NFY *)packet2.GetPacketData();

		res2->wOpCode = TU_GUILD_SECOND_MASTER_APPOINTED_NFY;
		res2->memberCharId = req->targetMemberCharId;
		//app->UserBroadcastothers(&packet2, this); // CHANGE THAT IT ONLY BROADCAST TO OTHER GUILD MEMBERS
		packet2.SetPacketLen( sizeof(sTU_GUILD_SECOND_MASTER_APPOINTED_NFY));
		rc = g_pApp->Send( this->GetHandle(), &packet2);

	}

}
//--------------------------------------------------------------------------------------//
//		Remove second guild master
//--------------------------------------------------------------------------------------//
void CClientSession::SendRemoveSecondGuildMaster(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_GUILD_DISMISS_SECOND_MASTER_REQ * req = (sUT_GUILD_DISMISS_SECOND_MASTER_REQ *)pPacket->GetPacketData();

	app->db->prepare("UPDATE guilds SET GuildSecondMaster = 0 WHERE GuildSecondMaster = ?");
	app->db->setInt(1, req->targetMemberCharId);
	app->db->execute();
	app->db->prepare("UPDATE guild_members SET is_secondguildmaster = 0 WHERE MemberID = ?");
	app->db->setInt(1, req->targetMemberCharId);
	app->db->execute();

	CNtlPacket packet(sizeof(sTU_GUILD_DISMISS_SECOND_MASTER_RES));
	sTU_GUILD_DISMISS_SECOND_MASTER_RES * res = (sTU_GUILD_DISMISS_SECOND_MASTER_RES *)packet.GetPacketData();
	res->wOpCode = TU_GUILD_DISMISS_SECOND_MASTER_RES;
	res->wResultCode = CHAT_SUCCESS;
	packet.SetPacketLen( sizeof(sTU_GUILD_DISMISS_SECOND_MASTER_RES));
	int rc = g_pApp->Send( this->GetHandle(), &packet);

	CNtlPacket packet2(sizeof(sTU_GUILD_SECOND_MASTER_DISMISSED_NFY));
	sTU_GUILD_SECOND_MASTER_DISMISSED_NFY * res2 = (sTU_GUILD_SECOND_MASTER_DISMISSED_NFY *)packet2.GetPacketData();
	res2->wOpCode = TU_GUILD_SECOND_MASTER_DISMISSED_NFY;
	res2->memberCharId = req->targetMemberCharId;
	packet2.SetPacketLen( sizeof(sTU_GUILD_SECOND_MASTER_DISMISSED_NFY));
	rc = g_pApp->Send( this->GetHandle(), &packet2);
}
//--------------------------------------------------------------------------------------//
//		Change Guild Master
//--------------------------------------------------------------------------------------//
void CClientSession::SendUpdateGuildMaster(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_GUILD_CHANGE_GUILD_MASTER_REQ * req = (sUT_GUILD_CHANGE_GUILD_MASTER_REQ *)pPacket->GetPacketData();

	app->db->prepare("CALL ChangeGuildMaster (?,?,?, @wResultCode)");
	app->db->setInt(1, req->targetMemberCharId);
	app->db->setString(2, this->GetGuildName().c_str());
	app->db->setInt(3, this->GetCharacterId());
	app->db->execute();

	app->db->execute("SELECT @wResultCode");
	app->db->fetch();

	CNtlPacket packet(sizeof(sTU_GUILD_CHANGE_GUILD_MASTER_RES));
	sTU_GUILD_CHANGE_GUILD_MASTER_RES * res = (sTU_GUILD_CHANGE_GUILD_MASTER_RES *)packet.GetPacketData();
	res->newMasterCharId = req->targetMemberCharId;
	res->wOpCode = TU_GUILD_CHANGE_GUILD_MASTER_RES;
	res->wResultCode = app->db->getInt("@wResultCode");
	packet.SetPacketLen( sizeof(sTU_GUILD_CHANGE_GUILD_MASTER_RES));
	int rc = g_pApp->Send( this->GetHandle(), &packet);

	if(app->db->getInt("@wResultCode") == CHAT_SUCCESS)
	{
		CNtlPacket packet2(sizeof(sTU_GUILD_GUILD_MASTER_CHANGED_NFY));
		sTU_GUILD_GUILD_MASTER_CHANGED_NFY * res2 = (sTU_GUILD_GUILD_MASTER_CHANGED_NFY *)packet2.GetPacketData();
		res2->wOpCode = TU_GUILD_GUILD_MASTER_CHANGED_NFY;
		res2->memberCharId = req->targetMemberCharId;
		packet2.SetPacketLen( sizeof(sTU_GUILD_GUILD_MASTER_CHANGED_NFY));
		rc = g_pApp->Send( this->GetHandle(), &packet2);
	}
}
//---------------------------------------------
//---Friend Add Luiz45
//---------------------------------------------
void CClientSession::SendAddFriend(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_FRIEND_ADD_REQ* req = (sUT_FRIEND_ADD_REQ*)pPacket->GetPacketData();
	
	CNtlPacket packet(sizeof(sTU_FRIEND_ADD_RES));
	sTU_FRIEND_ADD_RES* res = (sTU_FRIEND_ADD_RES*)packet.GetPacketData();
	
	char wsName[260];
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, req->wchName, -1, wsName, 260, &DefChar, NULL);

	std::string charName(wsName);

	int iCharId = 0;
	app->db->prepare("SELECT * FROM characters WHERE CharName = ?");
	app->db->setString(1, charName);
	app->db->execute();
	app->db->fetch();
	if (app->db->rowsCount() != 0)
	{
		iCharId = app->db->getInt("CharID");
		app->db->prepare("SELECT * FROM buddylist WHERE owner_id = ? AND friend_id = ? AND moveBlackList = 0");
		app->db->setInt(1,this->GetCharacterId());
		app->db->setInt(2,iCharId);
		app->db->execute();
		app->db->fetch();
		if (app->db->rowsCount() != 0)
		{
			res->wOpCode = TU_FRIEND_ADD_RES;
			res->wResultCode = COMMUNITY_FRIEND_CHAR_ARLEADY_ADDED;
		}
		else
		{
			app->csf->AddRemoveFriend(this->GetCharacterId(), iCharId, false);
			wcscpy(res->wchName, req->wchName);
			res->targetID = iCharId;
			res->wOpCode = TU_FRIEND_ADD_RES;
			res->wResultCode = CHAT_SUCCESS;			
		}
	}	
	else
	{
		res->wOpCode = TU_FRIEND_ADD_RES;
		res->wResultCode = COMMUNITY_FRIEND_CHAR_NOT_FOUND;
	}
	packet.SetPacketLen(sizeof(sTU_FRIEND_ADD_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//---------------------------------------------
//---Friend Del Luiz45
//---------------------------------------------
void CClientSession::SendDelFriend(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_FRIEND_DEL_REQ* req = (sUT_FRIEND_DEL_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sTU_FRIEND_DEL_RES));
	sTU_FRIEND_DEL_RES* res = (sTU_FRIEND_DEL_RES*)packet.GetPacketData();

	res->targetID = req->targetID;
	res->wOpCode = TU_FRIEND_DEL_RES;
	res->wResultCode = CHAT_SUCCESS;
	
	app->csf->AddRemoveFriend(this->GetCharacterId(), req->targetID, true);

	packet.SetPacketLen(sizeof(sTU_FRIEND_DEL_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//---------------------------------------------
//---Friend Move Luiz45
//---------------------------------------------
void CClientSession::SendMoveFriend(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_FRIEND_MOVE_REQ* req = (sUT_FRIEND_MOVE_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sTU_FRIEND_MOVE_RES));
	sTU_FRIEND_MOVE_RES* res = (sTU_FRIEND_MOVE_RES*)packet.GetPacketData();

	app->csf->AddRemoveFriend(this->GetCharacterId(), req->targetID, false,true);
	res->targetID = req->targetID;
	res->wOpCode = TU_FRIEND_MOVE_RES;
	res->wResultCode = CHAT_SUCCESS;

	packet.SetPacketLen(sizeof(sTU_FRIEND_MOVE_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//---------------------------------------------
//---Black List Add Luiz45
//---------------------------------------------
void CClientSession::SendBlackListAdd(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_FRIEND_BLACK_ADD_REQ* req = (sUT_FRIEND_BLACK_ADD_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sTU_FRIEND_BLACK_ADD_RES));
	sTU_FRIEND_BLACK_ADD_RES* res = (sTU_FRIEND_BLACK_ADD_RES*)packet.GetPacketData();

	char wsName[260];
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, req->awchName, -1, wsName, 260, &DefChar, NULL);

	std::string charName(wsName);

	int iCharId = 0;
	app->db->prepare("SELECT * FROM characters WHERE CharName = ?");
	app->db->setString(1, charName);
	app->db->execute();
	app->db->fetch();
	if (app->db->rowsCount() != 0)
	{
		iCharId = app->db->getInt("CharID");
		app->db->prepare("SELECT * FROM blacklist WHERE owner_id = ? AND target_id = ?");
		app->db->setInt(1, this->GetCharacterId());
		app->db->setInt(2, iCharId);
		app->db->execute();
		app->db->fetch();
		if (app->db->rowsCount() == 0)
		{
			app->csf->AddRemoveBlackList(this->GetCharacterId(), iCharId, false);
			wcscpy(res->wchName, req->awchName);
			res->targetID = iCharId;
			res->wOpCode = TU_FRIEND_BLACK_ADD_RES;
			res->wResultCode = CHAT_SUCCESS;
		}
		else
		{
			res->wOpCode = TU_FRIEND_BLACK_ADD_RES;
			res->wResultCode = COMMUNITY_FRIEND_BLACK_CHAR_EXIST;
		}
	}
	else
	{
		res->wOpCode = TU_FRIEND_BLACK_ADD_RES;
		res->wResultCode = COMMUNITY_FRIEND_CHAR_NOT_FOUND;
	}

	packet.SetPacketLen(sizeof(sTU_FRIEND_BLACK_ADD_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//---------------------------------------------
//---Black List Del Luiz45
//---------------------------------------------
void CClientSession::SendBlackListDel(CNtlPacket * pPacket, CChatServer * app)
{
	sUT_FRIEND_BLACK_DEL_REQ* req = (sUT_FRIEND_BLACK_DEL_REQ*)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sTU_FRIEND_BLACK_DEL_RES));
	sTU_FRIEND_BLACK_DEL_RES* res = (sTU_FRIEND_BLACK_DEL_RES*)packet.GetPacketData();

	res->targetID = req->targetID;
	res->wOpCode = TU_FRIEND_BLACK_DEL_RES;
	res->wResultCode = CHAT_SUCCESS;

	app->csf->AddRemoveBlackList(this->GetCharacterId(), req->targetID, true);

	packet.SetPacketLen(sizeof(sTU_FRIEND_BLACK_DEL_RES));
	g_pApp->Send(this->GetHandle(), &packet);
}
//---------------------------------------------
//---Friend List Info Luiz45
//---------------------------------------------
void CClientSession::SendFriendList(CNtlPacket * pPacket, CChatServer * app)
{
	CNtlPacket packet(sizeof(sTU_FRIEND_LIST_INFO));
	sTU_FRIEND_LIST_INFO* res = (sTU_FRIEND_LIST_INFO*)packet.GetPacketData();
	MySQLConnWrapper *db2 = new MySQLConnWrapper;
	db2->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
	db2->connect();
	db2->switchDb(app->GetConfigFileDatabase());

	app->db->prepare("SELECT * FROM buddylist WHERE owner_id = ?");
	app->db->setInt(1, this->GetCharacterId());
	app->db->execute();
	int iCounter = 0;
	while (app->db->fetch())
	{
		db2->prepare("SELECT * FROM characters WHERE CharID = ?");
		db2->setInt(1, app->db->getInt("friend_id"));
		db2->execute();
		while (db2->fetch())
		{
			res->asInfo[iCounter].bIsBlack = app->db->getInt("moveBlackList");
			res->asInfo[iCounter].charID = app->db->getInt("friend_id");
			wcscpy(res->asInfo[iCounter].wchName, s2ws(db2->getString("CharName")).c_str());
			iCounter++;
		}
	}
	app->db->prepare("SELECT * FROM blacklist WHERE owner_id = ?");
	app->db->setInt(1, this->GetCharacterId());
	app->db->execute();
	while (app->db->fetch())
	{
		db2->prepare("SELECT * FROM characters WHERE CharID = ?");
		db2->setInt(1, app->db->getInt("target_id"));
		db2->execute();
		while (db2->fetch())
		{
			res->asInfo[iCounter].bIsBlack = true;
			res->asInfo[iCounter].charID = app->db->getInt("target_id");
			wcscpy(res->asInfo[iCounter].wchName, s2ws(db2->getString("CharName")).c_str());
			iCounter++;
		}
	}
	res->byCount = iCounter;
	res->wOpCode = TU_FRIEND_LIST_INFO;
	packet.AdjustPacketLen(sizeof(sNTLPACKETHEADER)+(2 * sizeof(BYTE)) + (iCounter * (sizeof(sFRIEND_FULL_INFO))));
	g_pApp->Send(this->GetHandle(), &packet);
}