//-----------------------------------------------------------------------------------
//	 Gameserver
//-----------------------------------------------------------------------------------

#include "stdafx.h"
#include "GameServer.h"

#include <fstream>
using namespace std;


//-----------------------------------------------------------------------------------
CClientSession::~CClientSession()
{
	//NTL_PRINT(PRINT_APP, "CClientSession Destructor Called");
}


int CClientSession::OnAccept()
{
	//NTL_PRINT( PRINT_APP, "%s", __FUNCTION__ );
	this->isHandShaken = false;
	CNtlPacket packet(0x06);
	unsigned char buf[] = { 0x03, 0x00, 0xac, 0x86, 0xf5, 0x74 };
	packet.SetPacket(buf, 0x06);
	packet.GetPacketHeader()->bEncrypt = true;
	PushHandshakePacket(&packet);
	return NTL_SUCCESS;
}


void CClientSession::OnClose()
{
	//NTL_PRINT( PRINT_APP, "%s", __FUNCTION__ );	

}


int CClientSession::OnDispatch(CNtlPacket * pPacket)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();
	sNTLPACKETHEADER * pHeader = (sNTLPACKETHEADER *)pPacket->GetPacketData();
	if (pHeader->wOpCode != 1)
	//printf("~~~ opcode %i received ~~~ \n", pHeader->wOpCode);
	if (pHeader->wOpCode > 16)
	NTL_PRINT(PRINT_SYSTEM, "%s [%u] Length[%u] DataSize[%u]", NtlGetPacketName_UG(pHeader->wOpCode), pHeader->wOpCode, pPacket->GetPacketLen(), pPacket->GetUsedSize());

	switch( pHeader->wOpCode )
	{

	case 0x04:
	{
		if (this->isHandShaken == false)
		{
			CNtlPacket packet2(0x22);
			unsigned char buf2[] = { 0x10, 0x00, 0x84, 0xfb, 0x48, 0xf4, 0x8e, 0x5a, 0xb6, 0x67, 0xe2, 0x3d, 0x6e, 0x14, 0xb4, 0xa3, 0xc3, 0x24, 0x9e, 0x5f, 0xe3, 0xd1, 0xd5, 0x88, 0x10, 0x0d, 0x68, 0x4f, 0x3b, 0xa5, 0xed, 0x37, 0xed, 0x4a };
			packet2.SetPacket(buf2, 0x22);
			packet2.GetPacketHeader()->bEncrypt = false;
			PushHandshakePacket(&packet2);
			this->isHandShaken = true;
		}
	}
		break;
	case UG_PING:
	{
		CNtlPacket packet(sizeof(sGU_PONG));
		sGU_PONG* res = (sGU_PONG*)packet.GetPacketData();
		res->wOpCode = GU_PONG;
		packet.SetPacketLen(sizeof(sGU_PONG));
		g_pApp->Send(this->GetHandle(), &packet);
	}
		break;
		case UG_GAME_ENTER_REQ:
		{
			CClientSession::SendGameEnterReq(pPacket, app);
			CClientSession::SendAvatarCharInfo(pPacket, app);
			CClientSession::SendAvatarSkillInfo(pPacket, app);
			CClientSession::SendAvatarItemInfo(pPacket, app);
			CClientSession::SendAvatarBuffInfo(pPacket, app);
			CClientSession::SendAvatarHTBInfo(pPacket, app);
			//CClientSession::SendAvatarQuestList(pPacket, app);// Only uncomment when we get quests id
			CClientSession::SendSlotInfo(pPacket, app);
			CClientSession::SendBusLocation(pPacket, app);			
			CClientSession::SendAvatarInfoEnd(pPacket);
			CClientSession::SendAvatarWarFogInfo(pPacket, app);
			
			//CClientSession::SendUpdateToken(pPacket, app);

		}
			break;
		case UG_GAME_LEAVE_REQ:
		{
			CClientSession::SendGameLeaveReq(pPacket, app);
		}
			break;
		case UG_AUTH_KEY_FOR_COMMUNITY_SERVER_REQ:
		{
			CClientSession::SendAuthCommunityServer(pPacket, app);
		}
			break;
		case UG_ENTER_WORLD:
		{
			CClientSession::SendNetMarbleMemberShipNFY(pPacket,app);
			CClientSession::SendDragonballsEvent(pPacket, app);
			CClientSession::SendServerContents(pPacket, app);
			CClientSession::SendWorldEnterReq(pPacket, app);
			CClientSession::SendEnterWorldComplete(pPacket);
			CClientSession::SendNpcCreate(pPacket, app);
			CClientSession::SendMonsterCreate(pPacket, app);
			CClientSession::SendBudokaiState(pPacket, app);
		}
			break;
		case UG_SERVER_COMMAND:
		{
			CClientSession::RecvServerCommand(pPacket, app);
			printf("Recieved command from client \r\n");
		}
			break;
		case UG_LOADING_COMPLETE_NFY:
		{
			//	printf("--- UG_CHAR_READY_FOR_COMMUNITY_SERVER_NFY --- \n");
		}
			break;
			
		case UG_CHAR_READY_FOR_COMMUNITY_SERVER_NFY:
		{
		//	printf("--- UG_CHAR_READY_FOR_COMMUNITY_SERVER_NFY --- \n");
		}
			break;
		case UG_CHAR_READY_TO_SPAWN:
		{
			PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
			plr->SetCharState(3);
			CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
			sGU_UPDATE_CHAR_STATE* res = (sGU_UPDATE_CHAR_STATE*)packet.GetPacketData();
			res->handle = this->GetavatarHandle();
			res->sCharState.sCharStateBase.byStateID = CHARSTATE_STANDING;
			res->wOpCode = GU_UPDATE_CHAR_STATE;

			packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));
			
			PushHandshakePacket(&packet);
			//CClientSession::SendCharReadyReq(pPacket, app);
		}
			break;
		case UG_CHAR_READY:
		{
			CClientSession::SendCharReady(pPacket);
		}
			break;
		case UG_CHAR_AIR_DASH:
		{
			CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
			sGU_UPDATE_CHAR_STATE* res = (sGU_UPDATE_CHAR_STATE*)packet.GetPacketData();
			res->handle = this->GetavatarHandle();
			res->sCharState.sCharStateBase.byStateID = CHARSTATE_AIR_DASH_ACCEL;
			res->wOpCode = GU_UPDATE_CHAR_STATE;
			packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_STATE));

			PushHandshakePacket(&packet);
			CClientSession::SendAirDash(pPacket,app);
			//CClientSession::SendCharMove(pPacket, app);
		}
			break;
		case UG_CHAR_AIR_ACCEL:
		{
			printf("AIR CHAR ACCEL");
		}
			break;
		case UG_CHAR_MOVE:
		{
			CClientSession::SendCharMove(pPacket, app);
		}
			break;
		case UG_CHAR_AIR_JUMP:
		{
			CClientSession::SendAirJump(pPacket, app);
		}
			break;
		case UG_CHAR_AIR_MOVE:
		{
			CClientSession::SendCharMove(pPacket, app);
		}
			break;
		case UG_CHAR_DEST_MOVE:	
		{
			CClientSession::SendCharDestMove(pPacket, app);
		}
			break;
		case UG_CHAR_CHANGE_HEADING:
		{
			CClientSession::SendCharChangeHeading(pPacket, app);
		}
			break;
		case UG_CHAR_AIR_MOVE_SYNC:
		{
			CClientSession::SendCharMoveSync(pPacket, app);
			
		}
			break;
		case UG_CHAR_CHANGE_DIRECTION_ON_FLOATING:
		{
			CClientSession::SendCharChangeDirOnFloating(pPacket, app);
		}
			break;
		case UG_CHAR_FOLLOW_MOVE:
		{
			CClientSession::SendCharFollowMove(pPacket, app);
		}
			break;
		case UG_CHAR_DASH_KEYBOARD:
		{
			CClientSession::SendCharDashKeyBoard(pPacket, app);
		}
			break;
		case UG_CHAR_DASH_MOUSE:
		{
			CClientSession::SendCharDashMouse(pPacket, app);
		}
			break;
		case UG_CHAR_JUMP:
		{
			CClientSession::SendCharJump(pPacket, app);
		}
			break;
		case UG_CHAR_JUMP_END:
		{
			CNtlPacket packet(sizeof(sGU_CHAR_JUMP));
			sGU_CHAR_JUMP * res = (sGU_CHAR_JUMP *)packet.GetPacketData();
			res->wOpCode = GU_CHAR_JUMP_END;
			res->handle = this->GetavatarHandle();
			app->UserBroadcastothers(&packet, this);
		}
			break;
		case UG_CHAR_FALLING:
		{
			//CClientSession::SendCharFalling(pPacket, app);
		}
			break;
		case UG_CHAR_TOGG_SITDOWN:
		{			
			PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());
			if(plr->GetPlayerSitGetUp()==true)
				CClientSession::SendCharStandUp(pPacket, app);
			else
				CClientSession::SendCharSitDown(pPacket, app);
		}
			break;
		case UG_CHAR_TOGG_FIGHTING:
		{
			CClientSession::SendCharToggleFighting(pPacket, app);
			CClientSession::SendNetPyStart(pPacket, app);
			
		}
			break;
		//case UG_CHAR_TOGG_RUNNING:
		//{
		//	//printf("--- UG_CHAR_TOGG_RUNNING --- \n");
		//}
			break;
		case UG_CHAR_TARGET_SELECT:
		{
			CClientSession::SendCharTargetSelect(pPacket);
			//CClientSession::SendScouterIndicatorReq(pPacket, app);
		}
			break;
		case UG_CHAR_TARGET_INFO:
		{
			CClientSession::SendCharTargetInfo(pPacket);
		}
			break;
		case UG_CHAR_TARGET_FACING:
		{
			CClientSession::SendCharTargetFacing(pPacket);
			
		}
			break;
		case UG_CHAR_ATTACK_BEGIN:
		{
			CClientSession::SendAttackBegin(pPacket,app);
			

		}
			break;
		case UG_CHAR_ATTACK_END:
		{
			CClientSession::SendAttackEnd(pPacket,app);
								  
		}
			break;
		case UG_CHAR_CHARGE:
		{
			CClientSession::SendRpCharge(pPacket, app);
		}
			break;
		case UG_CHAR_BLOCK_MODE:
		{
			printf("---  UG_CHAR_BLOCK_MODE --- \n");
		}
			break;
		case UG_CHAR_TELEPORT_REQ:
		{
			printf("---  UG_CHAR_TELEPORT_REQ --- \n");
		}
			break;
		case UG_CHAR_BIND_REQ:
		{
			CClientSession::SendCharBindReq(pPacket,app);
		}
			break;
		case UG_CHAR_REVIVAL_REQ:
		{
			CClientSession::SendCharRevivalReq(pPacket, app);
		}
			break;
		case UG_CHAR_SERVER_CHANGE_REQ:
		{
			//printf("---  UG_CHAR_SERVER_CHANGE_REQ --- \n");//
			//CClientSession::SendServerChangeReq(pPacket, app);
			CClientSession::SendWorldEnterReq1(pPacket, app);
			CClientSession::SendEnterWorldComplete(pPacket);
			//CClientSession::SendNpcCreate(pPacket, app);
			//CClientSession::SendMonsterCreate(pPacket, app);
		}
			break;
		case UG_CHAR_CHANNEL_CHANGE_REQ:
		{
			printf("---  UG_CHAR_CHANNEL_CHANGE_REQ --- \n");
		}
			break;
		case UG_CHAR_EXIT_REQ:
		{
			CClientSession::SendCharExitReq(pPacket, app);
		}
			break;
		case UG_GAME_EXIT_REQ:
		{
			//printf("--- EXIT GAME REQUEST --- \n");
		}
			break;
		case UG_CHAR_AWAY_REQ:
		{
			CClientSession::SendCharAwayReq(pPacket, app);
		}
			break;
		case UG_CHAR_KEY_UPDATE_REQ:
		{
			printf("--- UG_CHAR_KEY_UPDATE_REQ --- \n");
		}
			break;
		case UG_CHAR_DIRECT_PLAY_ACK:
		{
			CClientSession::SendDirectPlay(pPacket, app);
		}
			break;
		case UG_CHAR_KNOCKDOWN_RELEASE_NFY:
		{
			printf("--- UG_CHAR_KNOCKDOWN_RELEASE_NFY --- \n");
		}
			break;
		case UG_CHAR_SKILL_REQ:
		{
			CClientSession::SendCharSkillRes(pPacket, app);
		}
			break;
		case UG_SKILL_TARGET_LIST:
		{
			printf("--- UG_SKILL_TARGET_LIST --- \n");
		}
			break;
		case UG_SKILL_UPGRADE_REQ:
		{
			CClientSession::SendCharSkillUpgrade(pPacket, app);
		}
			break;
		case UG_SKILL_RP_BONUS_SETTING_REQ:
		{
			printf("--- UG_SKILL_RP_BONUS_SETTING_REQ --- \n");
		}
			break;
		case UG_HTB_START_REQ:
		{
			CClientSession::SendHTBStartReq(pPacket, app);
		}
			break;
		case UG_HTB_LEARN_REQ:
		{
			CClientSession::SendCharSkillHTBLearn(pPacket, app);
		}
			break;
		case UG_HTB_FORWARD_REQ:
		{
			CClientSession::SendHTBFoward(pPacket, app);
		}
			break;
		case UG_HTB_RP_BALL_USE_REQ:
		{
			CClientSession::SendHTBRpBall(pPacket,app);
		}
			break;
		case UG_BUFF_DROP_REQ:
		{
			CClientSession::SendCharSkillBuffDrop(pPacket, app);
		}
			break;
		case UG_ITEM_MOVE_REQ:
		{
			CClientSession::SendItemMoveReq(pPacket, app);
		}
			break;
		case UG_ITEM_MOVE_STACK_REQ:
		{
			CClientSession::SendItemStackReq(pPacket, app);
		}
			break;
		case UG_ITEM_DELETE_REQ:
		{
			CClientSession::SendItemDeleteReq(pPacket, app);
		}
			break;
		case UG_ITEM_EQUIP_REPAIR_REQ:
		{
			printf("--- UG_ITEM_EQUIP_REPAIR_REQ --- \n");
		}
			break;
		case UG_ITEM_PICK_REQ:
		{
			CClientSession::SendItemPickUpReq(pPacket, app);
		}
			break;
		case UG_ITEM_REPAIR_REQ:
		{
			printf("--- UG_ITEM_REPAIR_REQ --- \n");
		}
			break;
		case UG_ITEM_USE_REQ:
		{
			CClientSession::SendItemUseReq(pPacket, app);
		}
			break;
		case UG_ITEM_UPGRADE_REQ:
		{
			printf("--- UG_ITEM_UPGRADE_REQ --- \n");
			CClientSession::SendItemUpgradeReq(pPacket, app);
		}
			break;
		case UG_ITEM_IDENTIFY_REQ:
		{
			printf("--- UG_ITEM_IDENTIFY_REQ --- \n");
		}
			break;
		case UG_ZENNY_PICK_REQ:
		{
			CClientSession::SendZennyPickUpReq(pPacket, app);
		}
			break;
		case UG_SHOP_START_REQ:
		{
			CClientSession::SendShopStartReq(pPacket, app);
		}
			break;
		case UG_SHOP_BUY_REQ:
		{
			CClientSession::SendShopBuyReq(pPacket, app);
		}
			break;
		case UG_SHOP_SELL_REQ:
		{
			CClientSession::SendShopSellReq(pPacket, app);
		}
			break;
		case UG_SHOP_END_REQ:
		{
			CClientSession::SendShopEndReq(pPacket, app);
		}
			break;
		case UG_SKILL_LEARN_REQ://fix by Marco Troquei pelo UG_BUY_SKILL_REQ
		{
			CClientSession::SendCharLearnSkillReq(pPacket, app);
		}
			break;
		case UG_SKILL_LEARN_BY_ITEM_REQ://fix by Marco Troquei pelo UG_BUY_SKILL_REQ
		{
			CClientSession::SendCharSkillByItemRes(pPacket, app); 
		}
			break;
		case UG_PARTY_CREATE_REQ:
		{
			CClientSession::SendCreatePartyReq(pPacket, app);
		}
			break;
		case UG_PARTY_DISBAND_REQ:
		{
			CClientSession::SendDisbandPartyReq(pPacket, app);
		}
			break;
		case UG_PARTY_INVITE_REQ:
		{
			CClientSession::SendPartyInviteReq(pPacket, app);
		}
			break;
		case UG_PARTY_INVITE_CHARID_REQ:
		{
			printf("--- UG_PARTY_INVITE_CHARID_REQ --- \n");
		}
			break;
		case UG_PARTY_INVITE_CHAR_NAME_REQ:
		{
			printf("--- UG_PARTY_INVITE_CHAR_NAME_REQ --- \n");
		}
			break;
		case UG_PARTY_RESPONSE_INVITATION:
		{
			CClientSession::SendPartyResponse(pPacket, app);
		}
			break;
		case UG_PARTY_LEAVE_REQ:
		{
			CClientSession::SendPartyLeaveReq(pPacket, app);
		}
			break;
		case UG_PARTY_KICK_OUT_REQ:
		{
			printf("--- UG_PARTY_KICK_OUT_REQ --- \n");
		}
			break;
		case UG_PARTY_CHANGE_LEADER_REQ:
		{
			printf("--- UG_PARTY_CHANGE_LEADER_REQ --- \n");
		}
			break;
		//case UG_PARTY_OPEN_CHARM_SLOT_REQ:
		//{
		//	printf("--- UG_PARTY_OPEN_CHARM_SLOT_REQ --- \n");
		//}
		//	break;
		//case UG_PARTY_CLOSE_CHARM_SLOT_REQ:
		//{
		//	printf("--- UG_PARTY_CLOSE_CHARM_SLOT_REQ --- \n");
		//}
		//	break;
		//case UG_PARTY_REGISTER_VICTIM_ITEM_REQ:
		//{
		//	printf("--- UG_SHOP_SKILL_BUY_REQ --- \n");
		//}
		//	break;
		//case UG_PARTY_UNREGISTER_VICTIM_ITEM_REQ:
		//{
		//	printf("--- UG_PARTY_UNREGISTER_VICTIM_ITEM_REQ --- \n");
		//}
		//	break;

		//case UG_PARTY_REGISTER_VICTIM_ZENNY_REQ:
		//{
		//	printf("--- UG_PARTY_REGISTER_VICTIM_ZENNY_REQ --- \n");
		//}
		//	break;
		//case UG_PARTY_UNREGISTER_VICTIM_ZENNY_REQ:
		//{
		//	printf("--- UG_PARTY_UNREGISTER_VICTIM_ZENNY_REQ --- \n");
		//}
		//	break;
		//case UG_PARTY_DECLARE_ZENNY_REQ:
		//{
		//	printf("--- UG_PARTY_DECLARE_ZENNY_REQ --- \n");
		//}
		//	break;
		//case UG_PARTY_ACTIVATE_CHARM_REQ:
		//{
		//	printf("--- UG_PARTY_ACTIVATE_CHARM_REQ --- \n");
		//}
		//	break;
		//case UG_PARTY_DEACTIVATE_CHARM_REQ:
		//{
		//	printf("--- UG_PARTY_DEACTIVATE_CHARM_REQ --- \n");
		//}
		//	break;
		case UG_PARTY_CHANGE_ZENNY_LOOTING_METHOD_REQ:
		{
			CClientSession::SendPartyChangeZenny(pPacket, app);
		}
			break;
		case UG_PARTY_CHANGE_ITEM_LOOTING_METHOD_REQ:
		{
			CClientSession::SendPartyChangeItem(pPacket, app);
		}
			break;
	/*	case UG_PARTY_INVEST_ZENNY_REQ:
		{
			printf("--- UG_PARTY_INVEST_ZENNY_REQ --- \n");
		}
			break;
		case UG_PARTY_INVEN_ITEM_RANK_SET_REQ:
		{
			printf("--- UG_PARTY_INVEN_ITEM_RANK_SET_REQ --- \n");
		}
			break;*/
		case UG_BANK_LOAD_REQ:
		{
			printf("--- UG_BANK_LOAD_REQ --- \n");
		}
			break;
		case UG_BANK_START_REQ:
		{
			CClientSession::SendBankStartReq(pPacket, app);
		}
			break;
		case UG_BANK_MOVE_REQ:
		{
			CClientSession::SendBankMoveReq(pPacket, app);
		}
			break;
		case UG_BANK_MOVE_STACK_REQ:
		{
			CClientSession::SendBankStackReq(pPacket, app);
		}
			break;
		case UG_BANK_END_REQ:
		{
			CClientSession::SendBankEndReq(pPacket, app);
		}
			break;
		case UG_BANK_ZENNY_REQ:
		{
			CClientSession::SendBankMoneyReq(pPacket, app);
		}
			break;
		case UG_BANK_BUY_REQ:
		{
			CClientSession::SendBankBuyReq(pPacket, app);
		}
			break;
		case UG_BANK_ITEM_DELETE_REQ:
		{
			CClientSession::SendBankDeleteReq(pPacket, app);
		}
			break;
		case UG_SCOUTER_INDICATOR_REQ:
		{
			CClientSession::SendScouterIndicatorReq(pPacket, app);
		}
			break;
		case UG_SCOUTER_PREDICT_REQ:
		{
			printf("--- UG_SCOUTER_PREDICT_REQ --- \n");
			
		}
			break;
		case UG_SCOUTER_EQUIP_CHECK_REQ:
		{
			printf("--- UG_SCOUTER_EQUIP_CHECK_REQ --- \n");
			
		}
			break;
		case UG_SCOUTER_CHIP_REMOVE_ALL_REQ:
		{
			printf("--- UG_SCOUTER_CHIP_REMOVE_ALL_REQ --- \n");
		}
			break;
		case UG_SOCIAL_ACTION:
		{
			CClientSession::SendSocialSkillRes(pPacket,app);
		}
			break;
		case UG_TS_CONFIRM_STEP_REQ:
		{
			CClientSession::SendPlayerQuestReq(pPacket, app);
		}
			break;
		case UG_TS_UPDATE_STATE:
		{
			CClientSession::SendTSUpdateState(pPacket, app);
		}
			break;
		case UG_TS_EXCUTE_TRIGGER_OBJECT:
		{
			CClientSession::SendExcuteTriggerObject(pPacket, app);
		}
			break;
		case UG_QUEST_ITEM_MOVE_REQ:
		{
			printf("--- UG_QUEST_ITEM_MOVE_REQ --- \n");
		}
			break;
		case UG_QUEST_ITEM_DELETE_REQ:
		{
			printf("--- UG_QUEST_ITEM_DELETE_REQ --- \n");
		}
			break;
		case UG_QUEST_GIVEUP_REQ:
		{
			printf("--- UG_QUEST_GIVEUP_REQ --- \n");
		}
			break;
		case UG_QUEST_SHARE:
		{
			printf("--- UG_QUEST_SHARE --- \n");
		}
			break;
		case UG_QUEST_OBJECT_VISIT_REQ:
		{
			CClientSession::SendObjectVisitQuest(pPacket, app);
		}
			break;
		case UG_FREEBATTLE_CHALLENGE_REQ:
		{
			CClientSession::SendFreeBattleReq(pPacket, app);
		}
			break;
		case UG_FREEBATTLE_ACCEPT_RES:
		{
			CClientSession::SendFreeBattleAccpetReq(pPacket, app);
		}
			break;
		case UG_QUICK_SLOT_UPDATE_REQ:
		{
			CClientSession::SendCharUpdQuickSlot(pPacket, app);
		}
			break;
		case UG_QUICK_SLOT_DEL_REQ:
		{
			CClientSession::SendCharDelQuickSlot(pPacket, app);
		}
			break;
		case UG_PET_DISMISS_PET_REQ:
		{
			printf("--- UG_PET_DISMISS_PET_REQ --- \n");
		}
			break;
		case UG_TRADE_START_REQ:
		{
			printf("--- UG_TRADE_START_REQ --- \n");
			CClientSession::SendTradeStartRes(pPacket, app);
		}
			break;
		case UG_TRADE_OK_RES:
		{
			printf("--- UG_TRADE_OK_RES --- \n");
			CClientSession::SendTradeOkRes(pPacket, app);
		}
			break;
		case UG_TRADE_ADD_REQ:
		{
			printf("--- UG_TRADE_ADD_REQ --- \n");
			CClientSession::SendTradeAddRes(pPacket, app);
		}
			break;
		case UG_TRADE_DEL_REQ:
		{
			printf("--- UG_TRADE_DEL_REQ --- \n");
			CClientSession::SendTradeDelRes(pPacket, app);
		}
			break;
		case UG_TRADE_MODIFY_REQ:
		{
			printf("--- UG_TRADE_MODIFY_REQ --- \n");
			CClientSession::SendTradeItemModify(pPacket, app);
		}
			break;
		case UG_TRADE_ZENNY_UPDATE_REQ:
		{
			printf("--- UG_TRADE_ZENNY_UPDATE_REQ --- \n");
			CClientSession::SendTradeZennyModify(pPacket, app);
		}
			break;
		case UG_TRADE_END_REQ:
		{
			printf("--- UG_TRADE_END_REQ --- \n");
			CClientSession::SendTradeFinish(pPacket, app);
		}
			break;
		case UG_TRADE_CANCEL_REQ:
		{
			printf("--- UG_TRADE_CANCEL_REQ --- \n");
			CClientSession::SendTradeCancelRes(pPacket, app);
		}
			break;
		case UG_TRADE_DENY_REQ:
		{
			printf("--- UG_TRADE_DENY_REQ --- \n");
			CClientSession::SendTradeDenyRes(pPacket, app);
		}
			break;

		case UG_GUILD_CREATE_REQ:
		{
			CClientSession::SendGuildCreateReq(pPacket, app);
		}
			break;
		case UG_GUILD_INVITE_REQ:
		{
			printf("--- UG_GUILD_INVITE_REQ --- \n");
		}
			break;
		case UG_TUTORIAL_HINT_UPDATE_REQ:
		{
			CClientSession::SendTutorialHintReq(pPacket, app);
		}
			break;
		case UG_TUTORIAL_WAIT_CANCEL_REQ:
		{
			printf("--- UG_TUTORIAL_WAIT_CANCEL_REQ --- \n");
		}
			break;
		case UG_TUTORIAL_PLAY_QUIT_REQ:
		{
			CClientSession::SendTutorialPlayQuit(pPacket, app);
		}
			break;
	/*	case UG_TIMEQUEST_ROOM_LIST_REQ:
		{
			CClientSession::SendTimeQuestList(pPacket, app);
		}
			break;
		case UG_TIMEQUEST_ROOM_INFO_REQ:
		{
			printf("--- UG_TIMEQUEST_ROOM_INFO_REQ --- \n");
		}
			break;
		case UG_TIMEQUEST_ROOM_JOIN_REQ:
		{
			CClientSession::JoinTimeQuestRoom(pPacket, app);
		}
			break;
		case UG_TIMEQUEST_ROOM_LEAVE_REQ:
		{
			CClientSession::LeaveTimeQuestRoom(pPacket, app);
		}
			break;
		case UG_TIMEQUEST_ROOM_TELEPORT_REQ:
		{
			CClientSession::SendTimeQuestTeleport(pPacket, app);
		}
			break;*/
		case UG_RANKBATTLE_INFO_REQ:
		{
			printf("--- UG_RANKBATTLE_INFO_REQ --- \n");
		}
			break;
		case UG_RANKBATTLE_JOIN_REQ:
		{
			printf("--- UG_RANKBATTLE_JOIN_REQ --- \n");
		}
			break;
		case UG_RANKBATTLE_LEAVE_REQ:
		{
			printf("--- UG_RANKBATTLE_LEAVE_REQ --- \n");
		}
			break;
		//case UG_RANKBATTLE_ROOM_LIST_REQ:
		//{
		//	printf("--- UG_RANKBATTLE_ROOM_LIST_REQ --- \n");
		//}
		//	break;
		//case UG_RANKBATTLE_ROOM_INFO_REQ:
		//{
		//	printf("--- UG_RANKBATTLE_ROOM_INFO_REQ --- \n");
		//}
		//	break;
		//case UG_RANKBATTLE_ROOM_CREATE_REQ:
		//{
		//	printf("--- UG_RANKBATTLE_ROOM_CREATE_REQ --- \n");
		//}
		//	break;
		//case UG_RANKBATTLE_ROOM_LEAVE_REQ:
		//{
		//	printf("--- UG_RANKBATTLE_ROOM_LEAVE_REQ --- \n");
		//}
		//	break;
		//case UG_RANKBATTLE_ROOM_CHALLENGE_REQ:
		//{
		//	printf("--- UG_RANKBATTLE_ROOM_CHALLENGE_REQ --- \n");
		//}
		//	break;
		//case UG_RANKBATTLE_ROOM_MATCH_CANCEL_REQ:
		//{
		//	printf("--- UG_RANKBATTLE_ROOM_MATCH_CANCEL_REQ --- \n");
		//}
		//	break;
		case UG_DRAGONBALL_CHECK_REQ:
		{
			CClientSession::SendDragonBallCheckReq(pPacket, app);
		}
			break;
		case UG_DRAGONBALL_REWARD_REQ:
		{
			CClientSession::SendDragonBallRewardReq(pPacket, app);
		}
			break;
		case UG_PRIVATESHOP_CREATE_REQ:
		{
			CClientSession::SendPrivateShopCreate(pPacket, app);
		}
			break;
		case UG_PRIVATESHOP_EXIT_REQ:
		{
			printf("--- UG_PRIVATESHOP_EXIT_REQ --- \n");
			CClientSession::SendPrivateShopExit(pPacket, app);
		}
			break;
		case UG_PRIVATESHOP_OPEN_REQ:
		{
			printf("--- UG_PRIVATESHOP_OPEN_REQ --- \n");
		}
			break;
		case UG_PRIVATESHOP_CLOSE_REQ:
		{
			printf("--- UG_PRIVATESHOP_CLOSE_REQ --- \n");
		}
			break;
		case UG_PRIVATESHOP_ITEM_UPDATE_REQ:
		{
			printf("--- UG_PRIVATESHOP_ITEM_UPDATE_REQ --- \n");
		}
			break;
		case UG_PRIVATESHOP_ITEM_INSERT_REQ:
		{
			printf("--- UG_PRIVATESHOP_ITEM_INSERT_REQ --- \n");
		}
			break;
		case UG_PRIVATESHOP_ITEM_GET_REQ:
		{
			printf("--- UG_PRIVATESHOP_ITEM_GET_REQ --- \n");
		}
			break;
		case UG_PRIVATESHOP_ITEM_DELETE_REQ:
		{
			printf("--- UG_PRIVATESHOP_ITEM_DELETE_REQ --- \n");
		}
			break;
		case UG_PRIVATESHOP_ITEM_BUYING_REQ:
		{
			printf("--- UG_PRIVATESHOP_ITEM_BUYING_REQ --- \n");
		}
			break;
		case UG_PRIVATESHOP_ENTER_REQ:
		{
			printf("--- UG_PRIVATESHOP_ENTER_REQ --- \n");
		}
			break;
		case UG_PRIVATESHOP_LEAVE_REQ:
		{
			printf("--- UG_PRIVATESHOP_LEAVE_REQ --- \n");
		}
			break;
		case UG_PRIVATESHOP_ITEM_SELECT_REQ:
		{
			printf("--- UG_PRIVATESHOP_ITEM_SELECT_REQ --- \n");
		}
			break;
		case UG_MAIL_START_REQ:
		{
			CClientSession::SendCharMailStart(pPacket, app);
		}
			break;
		case UG_MAIL_SEND_REQ:
		{
			CClientSession::SendCharMailSendReq(pPacket, app);
		}
			break;
		case UG_MAIL_READ_REQ:
		{
			CClientSession::SendCharMailReadReq(pPacket, app);
		}
			break;
		case UG_MAIL_DEL_REQ:
		{
			CClientSession::SendCharMailDelReq(pPacket, app);
		}
			break;
		case UG_MAIL_RETURN_REQ:
		{
			CClientSession::SendCharMailReturnReq(pPacket, app);
		}
			break;
		case UG_MAIL_RELOAD_REQ:
		{
			CClientSession::SendCharMailReloadReq(pPacket, app);
		}
			break;
		case UG_MAIL_LOAD_REQ:
		{
			CClientSession::SendCharMailLoadReq(pPacket, app);
		}
			break;
		case UG_MAIL_ITEM_RECEIVE_REQ:
		{
			CClientSession::SendCharMailItemReceiveReq(pPacket, app);
		}
			break;
		case UG_MAIL_LOCK_REQ:
		{
			CClientSession::SendCharMailLockReq(pPacket, app);
		}
			break;
		case UG_MAIL_MULTI_DEL_REQ:
		{
			CClientSession::SendCharMailMultiDelReq(pPacket, app);
		}
			break;
		case UG_PRIVATESHOP_BUSINESS_REQUEST_REQ:
		{
			printf("--- UG_PRIVATESHOP_BUSINESS_REQUEST_REQ --- \n");
		}
			break;
		case UG_PRIVATESHOP_BUSINESS_CONSENT_RES:
		{
			printf("--- UG_PRIVATESHOP_BUSINESS_CONSENT_RES --- \n");
		}
			break;
		case UG_PRIVATESHOP_BUSINESS_WAITTIMEOUT_NFY:
		{
			printf("--- UG_PRIVATESHOP_BUSINESS_WAITTIMEOUT_NFY --- \n");
		}
			break;
		case UG_PRIVATESHOP_BUSINESS_PRICEFLUCTUATIONS_REQ:
		{
			printf("--- UG_PRIVATESHOP_BUSINESS_PRICEFLUCTUATIONS_REQ --- \n");
		}
			break;
		case UG_PRIVATESHOP_BUSINESS_IMMEDIATE_ITEM_BUYING_REQ:
		{
			printf("--- UG_PRIVATESHOP_BUSINESS_IMMEDIATE_ITEM_BUYING_REQ --- \n");
		}
			break;
		case UG_PRIVATESHOP_BUSINESS_CANCEL_REQ:
		{
			printf("--- UG_PRIVATESHOP_BUSINESS_CANCEL_REQ --- \n");
		}
			break;
		case UG_PORTAL_START_REQ:
		{
			//CClientSession::SendPortalStartReq(pPacket, app);
		}
			break;
		case UG_PORTAL_ADD_REQ:
		{
			//CClientSession::SendPortalAddReq(pPacket, app);
			//CClientSession::SendPortalStartReq(pPacket, app);
		}
			break;
		case UG_PORTAL_REQ:
		{
			//CClientSession::SendPortalTelReq(pPacket, app);
		}
			break;
		case UG_WAR_FOG_UPDATE_REQ:
		{
			printf("--- UG_WAR_FOG_UPDATE_REQ --- \n");
			CClientSession::SendFogOfWarRes(pPacket, app);
		}
			break;
		case UG_GUILD_FUNCTION_ADD_REQ:
		{
			printf("--- UG_GUILD_FUNCTION_ADD_REQ --- \n");
			CClientSession::SendGuildFunctionAddReq(pPacket, app);
		}
			break;
		case UG_GUILD_GIVE_ZENNY:
		{
			printf("--- UG_GUILD_GIVE_ZENNY --- \n");
		}
			break;

		case UG_GUILD_BANK_START_REQ:
		{
			printf("--- UG_GUILD_BANK_START_REQ --- \n");
		}
			break;
		case UG_GUILD_BANK_MOVE_REQ:
		{
			printf("--- UG_GUILD_BANK_MOVE_REQ --- \n");
		}
			break;
		case UG_GUILD_BANK_MOVE_STACK_REQ:
		{
			printf("--- UG_GUILD_BANK_MOVE_STACK_REQ --- \n");
		}
			break;
		case UG_GUILD_BANK_END_REQ:
		{
			printf("--- UG_GUILD_BANK_END_REQ --- \n");
		}
			break;
		case UG_GUILD_BANK_ZENNY_REQ:
		{
			printf("--- UG_GUILD_BANK_ZENNY_REQ --- \n");
		}
			break;
		case UG_GUILD_CREATE_MARK_REQ:
		{
			printf("--- UG_GUILD_CREATE_MARK_REQ --- \n");
		}
			break;
		case UG_GUILD_CHANGE_MARK_REQ:
		{
			printf("--- UG_GUILD_CHANGE_MARK_REQ --- \n");
		}
			break;

		case UG_CROSSFIRE_REQ:
		{
			printf("--- UG_CROSSFIRE_REQ --- \n");
		}
			break;
		case UG_GUILD_CHANGE_NAME_REQ:
		{
			printf("--- UG_GUILD_CHANGE_NAME_REQ --- \n");
		}
			break;
		case UG_PARTY_SHARETARGET_REQ:
		{
			printf("--- UG_PARTY_SHARETARGET_REQ --- \n");
		}
			break;
		case UG_RIDE_ON_BUS_REQ:
		{
			printf("--- UG_RIDE_ON_BUS_REQ --- \n");
			CClientSession::SendRideOnBusRes(pPacket, app);
		}
			break;
		case UG_RIDE_OFF_BUS_REQ:
		{
			printf("--- UG_RIDE_OFF_BUS_REQ --- \n");
			CClientSession::SendRideOffBusRes(pPacket, app);
		}
			break;
		case UG_TRANSFORM_CANCEL_REQ:
		{
			printf("--- UG_TRANSFORM_CANCEL_REQ --- \n");
			CClientSession::SendCharSkillTransformCancel(pPacket, app);
		}
			break;
		case UG_SHOP_ITEM_IDENTIFY_REQ:
		{
			printf("--- UG_SHOP_ITEM_IDENTIFY_REQ --- \n");
		}
			break;
		case UG_DICE_ROLL_REQ:
		{
			CClientSession::SendRollDice(pPacket, app);
		}
			break;
		case UG_BUDOKAI_JOIN_INDIVIDUAL_REQ:
		{
			printf("--- UG_BUDOKAI_JOIN_INDIVIDUAL_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_LEAVE_INDIVIDUAL_REQ:
		{
			printf("--- UG_BUDOKAI_LEAVE_INDIVIDUAL_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_JOIN_TEAM_INFO_REQ:
		{
			printf("--- UG_BUDOKAI_JOIN_TEAM_INFO_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_JOIN_TEAM_REQ:
		{
			printf("--- UG_BUDOKAI_JOIN_TEAM_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_LEAVE_TEAM_REQ:
		{
			printf("--- UG_BUDOKAI_LEAVE_TEAM_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_LEAVE_TEAM_MEMBER_REQ:
		{
			printf("--- UG_BUDOKAI_LEAVE_TEAM_MEMBER_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_JOIN_INFO_REQ:
		{
			printf("--- UG_BUDOKAI_JOIN_INFO_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_JOIN_STATE_REQ:
		{
			printf("--- UG_BUDOKAI_JOIN_STATE_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_MUDOSA_INFO_REQ:
		{
			printf("--- UG_BUDOKAI_MUDOSA_INFO_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_MUDOSA_TELEPORT_REQ:
		{
			printf("--- UG_BUDOKAI_MUDOSA_TELEPORT_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_PARTY_MAKER_REQ:
		{
			printf("--- UG_BUDOKAI_PARTY_MAKER_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_SOCIAL_ACTION:
		{
			printf("--- UG_BUDOKAI_SOCIAL_ACTION --- \n");
		}
			break;
		case UG_BUDOKAI_PRIZEWINNER_NAME_REQ:
		{
			printf("--- UG_BUDOKAI_PRIZEWINNER_NAME_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_GM_BUDOKAI_SERVER_ENTER_REQ:
		{
			printf("--- UG_BUDOKAI_GM_BUDOKAI_SERVER_ENTER_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_GM_BUDOKAI_SERVER_LEAVE_REQ:
		{
			printf("--- UG_BUDOKAI_GM_BUDOKAI_SERVER_LEAVE_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_GM_MATCH_PROGRESS_INFO_REQ:
		{
			printf("--- UG_BUDOKAI_GM_MATCH_PROGRESS_INFO_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_GM_MATCH_ARENA_ENTER_REQ:
		{
			printf("--- UG_BUDOKAI_GM_MATCH_ARENA_ENTER_REQ --- \n");
		}
			break;
		case UG_BUDOKAI_GM_MATCH_ARENA_LEAVE_REQ:
		{
			printf("--- UG_BUDOKAI_GM_MATCH_ARENA_LEAVE_REQ --- \n");
		}
			break;
		case UG_SCOUTER_TURN_ON:
		{
			printf("--- UG_SCOUTER_TURN_ON --- \n");
			//CClientSession::SendScouterTurnOnReq(pPacket, app);
		}
			break;
		case UG_SCOUTER_TURN_OFF:
		{
			printf("--- UG_SCOUTER_TURN_OFF --- \n");
			//CClientSession::SendScouterTurnOffReq(pPacket, app);
		}
			break;
		case UG_SCOUTER_ITEM_SELL_REQ:
		{
			printf("--- UG_SCOUTER_ITEM_SELL_REQ --- \n");
		}
			break;
		case UG_SHOP_EVENTITEM_START_REQ:
		{
			printf("--- UG_SHOP_EVENTITEM_START_REQ --- \n");
			CNtlPacket packet(sizeof(sUG_SHOP_EVENTITEM_START_REQ));
			sUG_SHOP_EVENTITEM_START_REQ* res = (sUG_SHOP_EVENTITEM_START_REQ*)packet.GetPacketData();
			PlayersMain* plr = g_pPlayerManager->GetPlayer(this->GetavatarHandle());

			res->handle = plr->GetAvatarHandle();
			res->wOpCode = UG_SHOP_EVENTITEM_START_REQ;
			packet.SetPacketLen(sizeof(sUG_SHOP_EVENTITEM_START_REQ));
			g_pApp->Send(this->GetHandle(), &packet);
		}
			break;
		case UG_SHOP_EVENTITEM_BUY_REQ:
		{
			printf("--- UG_SHOP_EVENTITEM_BUY_REQ --- \n");
		}
			break;
		case UG_SHOP_EVENTITEM_END_REQ:
		{
			printf("--- UG_SHOP_EVENTITEM_END_REQ --- \n");
		}
			break;
		case UG_SHOP_GAMBLE_BUY_REQ:
		{
			CClientSession::SendGambleBuyReq(pPacket, app);
		}
			break;
		case UG_PARTY_DIFF_CHANGE_REQ:
		{
			printf("--- UG_PARTY_DIFF_CHANGE_REQ --- \n");
			//CClientSession::SendPartyChangeDiff(pPacket, app);
		}
			break;
		case UG_PARTY_DUNGEON_INIT_REQ:
		{
			printf("--- UG_PARTY_DUNGEON_INIT_REQ --- \n");
			//CClientSession::SendInitPartyDungeon(pPacket, app);
		}
			break;
		case UG_SKILL_INIT_REQ:
		{
			printf("--- UG_SKILL_INIT_REQ --- \n");
		}
			break;
		//case UG_PARTY_CHARM_DICE_ROLL_REQ:
		//{
		//	printf("--- UG_PARTY_CHARM_DICE_ROLL_REQ --- \n");
		//}
		//	break;
		//case UG_PARTY_INVEN_LOCK_REQ:
		//{
		//	printf("--- UG_PARTY_INVEN_LOCK_REQ --- \n");
		//}
		//	break;
		//case UG_PARTY_INVEN_RESET_REQ:
		//{
		//	printf("--- UG_PARTY_INVEN_RESET_REQ --- \n");
		//}
		//	break;
		//case UG_PARTY_INVEN_ACCEPTED_REQ:
		//{
		//	printf("--- UG_PARTY_INVEN_ACCEPTED_REQ --- \n");
		//}
		//	break;
		//case UG_PARTY_INVEN_GIVE_REQ:
		//{
		//	printf("--- UG_PARTY_INVEN_GIVE_REQ --- \n");
		//}
		//	break;
		//case UG_PARTY_INVEN_DICE_START_REQ:
		//{
		//	printf("--- UG_PARTY_INVEN_DICE_START_REQ --- \n");
		//}
		//	break;
		//case UG_PARTY_INVEN_DICE_ROLL_REQ:
		//{
		//	printf("--- UG_PARTY_INVEN_DICE_ROLL_REQ --- \n");
		//}
		//	break;
		case UG_TELEPORT_CONFIRM_REQ:
		{
			printf("--- UG_TELEPORT_CONFIRM_REQ --- \n");
		}
			break;
		case UG_HOIPOIMIX_ITEM_CHECK_REQ:
		{
			printf("--- UG_HOIPOIMIX_ITEM_CHECK_REQ --- \n");
		}
			break;
		case UG_HOIPOIMIX_ITEM_MAKE_REQ:
		{
			printf("--- UG_HOIPOIMIX_ITEM_MAKE_REQ --- \n");
		}
			break;
		case UG_HOIPOIMIX_ITEM_MAKE_EP_REQ:
		{
			printf("--- UG_HOIPOIMIX_ITEM_MAKE_EP_REQ --- \n");
		}
			break;
	/*	case UG_HOIPOIMIX_ITEM_MACHINE_MAKE_REQ:
		{
			printf("--- UG_HOIPOIMIX_ITEM_MACHINE_MAKE_REQ --- \n");
		}*/
			break;
		case UG_HOIPOIMIX_ITEM_MACHINE_DEL_REQ:
		{
			printf("--- UG_HOIPOIMIX_ITEM_MACHINE_DEL_REQ --- \n");
		}
			break;
		case UG_HOIPOIMIX_JOB_SET_REQ:
		{
			printf("--- UG_HOIPOIMIX_JOB_SET_REQ --- \n");
			//CClientSession::SendHoiPoiJob(pPacket, app);
		}
			break;
		case UG_HOIPOIMIX_JOB_RESET_REQ:
		{
			printf("--- UG_HOIPOIMIX_JOB_RESET_REQ --- \n");
			//CClientSession::SendHoiPoiJobReset(pPacket, app);
		}
			break;
		case UG_VEHICLE_DIRECT_PLAY_CANCEL_NFY:
		{
			printf("--- UG_VEHICLE_DIRECT_PLAY_CANCEL_NFY --- \n");
		}
			break;
		case UG_VEHICLE_ENGINE_START_REQ:
		{
			printf("--- UG_VEHICLE_ENGINE_START_REQ --- \n");
		}
			break;
		case UG_VEHICLE_ENGINE_STOP_REQ:
		{
			printf("--- UG_VEHICLE_ENGINE_STOP_REQ --- \n");
		}
			break;
		case UG_VEHICLE_END_REQ:
		{
			printf("--- UG_VEHICLE_END_REQ --- \n");
		}
			break;
		case UG_VEHICLE_STUNT_NFY:
		{
			printf("--- UG_VEHICLE_STUNT_NFY --- \n");
		}
			break;
		case UG_VEHICLE_FUEL_REMOVE_NFY:
		{
			printf("--- UG_VEHICLE_FUEL_REMOVE_NFY --- \n");
		}
			break;
	/*	case UG_VEHICLE_PASSENGER_GETON_REQ:
		{
			printf("--- UG_VEHICLE_PASSENGER_GETON_REQ --- \n");
		}
			break;
		case UG_VEHICLE_ASK_RES:
		{
			printf("--- UG_VEHICLE_ASK_RES --- \n");
		}
			break;
		case UG_VEHICLE_PASSENGER_GETOFF_REQ:
		{
			printf("--- UG_VEHICLE_PASSENGER_GETOFF_REQ --- \n");
		}
			break;*/
		case UG_DOJO_CREATE_REQ:
		{
			printf("--- UG_DOJO_CREATE_REQ --- \n");
		}
			break;
		case UG_DOJO_DELETE_REQ:
		{
			printf("--- UG_DOJO_DELETE_REQ --- \n");
		}
			break;
		case UG_DOJO_FUNCTION_ADD_REQ:
		{
			printf("--- UG_DOJO_FUNCTION_ADD_REQ --- \n");
		}
			break;
		case UG_DOJO_BANK_HISTORY_REQ:
		{
			printf("--- UG_DOJO_BANK_HISTORY_REQ --- \n");
		}
			break;
		case UG_DOGI_CREATE_REQ:
		{
			printf("--- UG_DOGI_CREATE_REQ --- \n");
		}
			break;
		case UG_DOGI_CHANGE_REQ:
		{
			printf("--- UG_DOGI_CHANGE_REQ --- \n");
		}
			break;
		case UG_GUILD_DOGI_CREATE_REQ:
		{
			printf("--- UG_GUILD_DOGI_CREATE_REQ --- \n");
		}
			break;
		case UG_GUILD_DOGI_CHANGE_REQ:
		{
			printf("--- UG_GUILD_DOGI_CHANGE_REQ --- \n");
		}
			break;
		case UG_DOJO_SCRAMBLE_REQ:
		{
			printf("--- UG_DOJO_SCRAMBLE_REQ --- \n");
		}
			break;
		case UG_DOJO_SCRAMBLE_RESPONSE_REQ:
		{
			printf("--- UG_DOJO_SCRAMBLE_RESPONSE_REQ --- \n");
		}
			break;
		case UG_SHOP_NETPYITEM_START_REQ:
		{
			printf("--- UG_SHOP_NETPYITEM_START_REQ --- \n");
			//CClientSession::SendNetPyStart(pPacket, app);
			//CClientSession::SendNetPyEnd(pPacket, app);//provisory
		}
			break;
		case UG_SHOP_NETPYITEM_BUY_REQ:
		{
			printf("--- UG_SHOP_NETPYITEM_BUY_REQ --- \n");
			//CClientSession::SendNetPyBuy(pPacket, app);
		}
			break;
		case UG_SHOP_NETPYITEM_END_REQ:
		{
			printf("--- UG_SHOP_NETPYITEM_END_REQ --- \n");
			CClientSession::SendNetPyEnd(pPacket, app);
		}
			break;
		case UG_WORLD_MAP_STATUS:
		{
			printf("--- UG_DOJO_NPC_INFO_REQ --- \n");
			sUG_WORLD_MAP_STATUS * req = (sUG_WORLD_MAP_STATUS*)pPacket->GetPacketData();
			req->bIsWorldMapOpen = true;

		}
			break;
		case UG_DOJO_NPC_INFO_REQ:
		{
			printf("--- UG_DOJO_NPC_INFO_REQ --- \n");
		}
			break;
		case UG_GAMEGUARD_AUTH_ANSWER_RES:
		{
			printf("--- UG_GAMEGUARD_AUTH_ANSWER_RES --- \n");
		}
			break;
		case UG_GAMEGUARD_HACK_INFO_NFY:
		{
			printf("--- UG_GAMEGUARD_HACK_INFO_NFY --- \n");
		}
			break;
		case UG_SKILL_CASTING_CANCELED_NFY:
		{
			printf("--- UG_SKILL_CASTING_CANCELED_NFY --- \n");
		}
			break;
		case UG_PET_ATTACK_TARGET_NFY:
		{
			printf("--- UG_PET_ATTACK_TARGET_NFY --- \n");
		}
			break;
		case UG_DURATION_ITEM_BUY_REQ:
		{
			printf("--- UG_DURATION_ITEM_BUY_REQ --- \n");
		}
			break;
		case UG_DURATION_RENEW_REQ:
		{
			printf("--- UG_DURATION_RENEW_REQ --- \n");
		}
			break;
		case UG_CASHITEM_START_REQ:
		{
			printf("--- UG_CASHITEM_START_REQ --- \n");
			CClientSession::SendCashItemStart(pPacket, app);
		}
			break;
		case UG_CASHITEM_END_REQ:
		{
			printf("--- UG_CASHITEM_END_REQ --- \n");
			CClientSession::SendCashItemEnd(pPacket, app);
		}
			break;
		case UG_CASHITEM_MOVE_REQ:
		{
			printf("--- UG_CASHITEM_MOVE_REQ --- \n");
			//CClientSession::SendCashItemMove(pPacket, app);
		}
			break;
		case UG_CASHITEM_USE_REQ:
		{
			printf("--- UG_CASHITEM_USE_REQ --- \n");
			//CClientSession::SendCashItemUse(pPacket, app);
		}
			break;
		case UG_CASHITEM_HLSHOP_START_REQ:
		{
			printf("--- UG_CASHITEM_HLSHOP_START_REQ --- \n");
			CClientSession::SendCashItemHlsStart(pPacket, app);
		}
			break;
		case UG_CASHITEM_HLSHOP_END_REQ:
		{
			printf("--- UG_CASHITEM_HLSHOP_END_REQ --- \n");
			CClientSession::SendCashItemHlsEnd(pPacket, app);
		}
			break;
		case UG_CASHITEM_HLSHOP_REFRESH_REQ:
		{
			printf("--- UG_CASHITEM_HLSHOP_END_REQ --- \n");
			//CClientSession::SendCashItemHlsRefresh(pPacket, app);
		}
		break;
		case UG_CASHITEM_BUY_REQ:
		{
			printf("--- UG_CASHITEM_BUY_REQ --- \n");
			//CClientSession::SendCashItemBuy(pPacket, app);
		}
		break;
		case UG_CASHITEM_SEND_GIFT_REQ:
		{
			printf("--- UG_CASHITEM_BUY_REQ --- \n");
			//CClientSession::SendCashItemSendGift(pPacket, app);
		}
		break;
		case UG_BUDOKAI_PRIZEWINNER_TEAM_NAME_REQ:
		{
			printf("--- UG_BUDOKAI_PRIZEWINNER_TEAM_NAME_REQ --- \n");
		}
			break;
		case UG_GMT_UPDATE_REQ:
		{
			printf("--- UG_GMT_UPDATE_REQ --- \n");
			CClientSession::SendGmtUpdateReq(pPacket, app);
		}
			break;
		case UG_CHAR_LOC_AFTER_KNOCKDOWN_NFY:
		{
			printf("--- UG_CHAR_LOC_AFTER_KNOCKDOWN_NFY --- \n");
		}
			break;
		case UG_CHAR_LOC_AFTER_SLIDING_NFY:
		{
			printf("--- UG_CHAR_LOC_AFTER_SLIDING_NFY --- \n");
		}
			break;
		case UG_CHAR_LOC_AFTER_PUSH_NFY:
		{
			printf("--- UG_CHAR_LOC_AFTER_PUSH_NFY --- \n");
		}
			break;
		case UG_GIFT_SHOP_START_REQ:
		{
			CClientSession::SenGiftShop(pPacket, app);
			CClientSession::SendCharReadyReq(pPacket, app);
			printf("--- UG_GIFT_SHOP_START_REQ --- \n");
		}
			break;
		case UG_SCOUTER_ACTIVATION_REQ:
		{
				
			
		}
			break;
		case UG_PERFORMANCE_DATA_NFY:
		case UG_PERFORMANCE_DATA_2_NFY:
		case UG_CHAR_DEST_MOVE_SYNC:
		case UG_CHAR_NPCSERVER_MOVE_SYNC:
		case UG_CHAR_LOCATION_SYNC:
		case UG_CHAR_STANDING_SLIP:
		case UG_CHAR_STANDING_SLIP_END:
		{
			//printf("--- bs--- \n");
		}
			break;

		default:
		{

			return CNtlSession::OnDispatch(pPacket);
		}
	}
	return NTL_SUCCESS;
}

//-----------------------------------------------------------------------------------
//		Loading Tables
//-----------------------------------------------------------------------------------
bool CGameServer::CreateTableContainer(int byLoadMethod)
{
	GsFunctionsClass *gs = new GsFunctionsClass();
	gs->printOk("==== LOADING GAME TABLES ... ====");

	CNtlBitFlagManager flagManager;
	if (false == flagManager.Create(CTableContainer::TABLE_COUNT))
	{
		return false;
	}

	CTableFileNameList fileNameList;
	if (false == fileNameList.Create())
	{
		return false;
	}
	/*Loadable Tables*/
	flagManager.Set(CTableContainer::TABLE_PC);
	flagManager.Set(CTableContainer::TABLE_NEWBIE);
	flagManager.Set(CTableContainer::TABLE_SKILL);
	/*Tables in Progress*/
	flagManager.Set(CTableContainer::TABLE_ITEM);	//Table loads. Data is incorrect after first 60 bytes			
	flagManager.Set(CTableContainer::TABLE_DRAGONBALL_REWARD);
	flagManager.Set(CTableContainer::TABLE_EXP);
	flagManager.Set(CTableContainer::TABLE_PORTAL);	
    flagManager.Set(CTableContainer::TABLE_USE_ITEM);			
	
	//flagManager.Set(CTableContainer::TABLE_MERCHANT);			


	/*Unloadable Tables*/
	//	 flagManager.Set(CTableContainer::TABLE_WORLD);								
	//   flagManager.Set(CTableContainer::TABLE_MOB);				
	//   flagManager.Set(CTableContainer::TABLE_NPC);				
	//   flagManager.Set(CTableContainer::TABLE_ITEM_OPTION);							
	//   flagManager.Set(CTableContainer::TABLE_SYSTEM_EFFECT);		
	//   flagManager.Set(CTableContainer::TABLE_HTB_SET);			
	//   flagManager.Set(CTableContainer::TABLE_SET_ITEM);			
	//   flagManager.Set(CTableContainer::TABLE_CHARM);				
	//   flagManager.Set(CTableContainer::TABLE_ACTION);				
	//   flagManager.Set(CTableContainer::TABLE_CHAT_COMMAND);		
	//   flagManager.Set(CTableContainer::TABLE_QUEST_ITEM);			
	//   flagManager.Set(CTableContainer::TABLE_QUEST_TEXT_DATA);	
	//   flagManager.Set(CTableContainer::TABLE_TEXT_ALL);
	//   flagManager.Set(CTableContainer::TABLE_OBJECT);			
	//   flagManager.Set(CTableContainer::TABLE_WORLD_MAP);			
	//   flagManager.Set(CTableContainer::TABLE_LAND_MARK);			
	//   flagManager.Set(CTableContainer::TABLE_HELP);				
	//   flagManager.Set(CTableContainer::TABLE_GUIDE_HINT);			
	//   flagManager.Set(CTableContainer::TABLE_DRAGONBALL);			
	//   flagManager.Set(CTableContainer::TABLE_DRAGONBALL_REWARD);	
	//   flagManager.Set(CTableContainer::TABLE_TIMEQUEST);			
	//   flagManager.Set(CTableContainer::TABLE_BUDOKAI);			
	//   flagManager.Set(CTableContainer::TABLE_RANKBATTLE);			
	//   flagManager.Set(CTableContainer::TABLE_DIRECTION_LINK);		
	//   flagManager.Set(CTableContainer::TABLE_CHATTING_FILTER);	
	//   flagManager.Set(CTableContainer::TABLE_SPEECH);				
	//   flagManager.Set(CTableContainer::TABLE_SCRIPT_LINK);		
	//   flagManager.Set(CTableContainer::TABLE_QUEST_NARRATION);	
	//   flagManager.Set(CTableContainer::TABLE_VEHICLE);			
	//   flagManager.Set(CTableContainer::TABLE_DUNGEON);			
	//   flagManager.Set(CTableContainer::TABLE_MOB_MOVE_PATTERN);	
	//   flagManager.Set(CTableContainer::TABLE_DYNAMIC_OBJECT);		
	//   flagManager.Set(CTableContainer::TABLE_ITEM_RECIPE);		
	//   flagManager.Set(CTableContainer::TABLE_ITEM_UPGRADE);		
	//   flagManager.Set(CTableContainer::TABLE_MIX_MACHINE);		
	//   flagManager.Set(CTableContainer::TABLE_DOJO);				
	//   flagManager.Set(CTableContainer::TABLE_QUEST_REWARD);		
	//   flagManager.Set(CTableContainer::TABLE_WORLD_ZONE);
	//   flagManager.Set(CTableContainer::TABLE_NPC_SPAWN);
	//   flagManager.Set(CTableContainer::TABLE_MOB_SPAWN);
	//   flagManager.Set(CTableContainer::TABLE_FORMULA);
	//   flagManager.Set(CTableContainer::TABLE_GAME_MANIA_TIME);
	//   flagManager.Set(CTableContainer::TABLE_BASIC_DROP);
	//   flagManager.Set(CTableContainer::TABLE_LEGENDARY_DROP);
	//   flagManager.Set(CTableContainer::TABLE_NORMAL_DROP);
	//   flagManager.Set(CTableContainer::TABLE_SUPERIOR_DROP);
	//   flagManager.Set(CTableContainer::TABLE_EACH_DROP);
	//   flagManager.Set(CTableContainer::TABLE_TYPE_DROP);
	//   flagManager.Set(CTableContainer::TABLE_EXCELLENT_DROP);
	//   flagManager.Set(CTableContainer::TABLE_QUEST_DROP);
	//   

	/*Loadable Tables*/
	fileNameList.SetFileName(CTableContainer::TABLE_PC,						"Table_PC_Data");
	fileNameList.SetFileName(CTableContainer::TABLE_NEWBIE,					"Table_Newbie_Data");
	fileNameList.SetFileName(CTableContainer::TABLE_SKILL,					"Table_Skill_Data");

	/*Tables in progress
	* If tables are here they load but don't have correct structures. 
	* Just a correct size. 
	*/
	fileNameList.SetFileName(CTableContainer::TABLE_ITEM,					"Table_Item_Data");
	fileNameList.SetFileName(CTableContainer::TABLE_DRAGONBALL_REWARD,		"Table_DB_Reward_Data");
	fileNameList.SetFileName(CTableContainer::TABLE_EXP,					"table_exp_data");
	fileNameList.SetFileName(CTableContainer::TABLE_PORTAL,					"Table_Portal_Data");
	fileNameList.SetFileName(CTableContainer::TABLE_USE_ITEM,				"Table_Use_Item_Data");
	
	//fileNameList.SetFileName(CTableContainer::TABLE_MERCHANT,				"Table_Merchant_Data");


	/*Unloadable Tables*/
	//fileNameList.SetFileName(CTableContainer::TABLE_WORLD,					"Table_World_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_MOB,					"Table_MOB_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_NPC,					"Table_NPC_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_ITEM_OPTION,			"Table_Item_Option_Data");	
	//fileNameList.SetFileName(CTableContainer::TABLE_SYSTEM_EFFECT,			"Table_System_Effect_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_HTB_SET,				"Table_HTB_Set_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_SET_ITEM,				"Table_Set_Item_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_CHARM,					"Table_Charm_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_ACTION,					"Table_Action_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_CHAT_COMMAND,			"Table_Chat_Command_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_QUEST_ITEM,				"Table_Quest_Item_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_QUEST_TEXT_DATA,		"Table_Quest_Text_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_TEXT_ALL,				"Table_Text_All_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_OBJECT,					"Table_Object");
	//fileNameList.SetFileName(CTableContainer::TABLE_WORLD_MAP,				"Table_Worldmap_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_LAND_MARK,				"Table_Landmark_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_HELP,					"Table_Help_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_GUIDE_HINT,				"Table_Guide_Hint_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_DRAGONBALL,				"Table_Dragon_Ball_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_TIMEQUEST,				"Table_TMQ_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_BUDOKAI,				"Table_Tenkaichibudokai_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_RANKBATTLE,				"Table_RankBattle_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_DIRECTION_LINK,			"Table_Direction_Link_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_CHATTING_FILTER,		"Table_Chatting_Filter_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_SPEECH,					"Table_NPC_Speech_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_SCRIPT_LINK,			"Table_Script_Link_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_QUEST_NARRATION,		"Table_Quest_Narration_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_VEHICLE,				"Table_Vehicle_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_DUNGEON,				"Table_Dungeon_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_MOB_MOVE_PATTERN,		"Table_Mob_Move_Pattern_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_DYNAMIC_OBJECT,			"Table_Dynamic_Object_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_ITEM_RECIPE,			"Table_Item_Recipe_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_ITEM_UPGRADE,			"Table_Item_Upgrade_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_MIX_MACHINE,			"Table_Item_Mix_Machine_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_DOJO,					"Table_Dojo_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_QUEST_REWARD,			"Table_Quest_Reward_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_WORLD_ZONE,				"Table_World_Zone_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_FORMULA,				"TD_Formula");
	//fileNameList.SetFileName(CTableContainer::TABLE_GAME_MANIA_TIME,		"Table_GameManiaTime_Data");
	//fileNameList.SetFileName(CTableContainer::TABLE_BASIC_DROP,				"table_basic_drop_data");
	//fileNameList.SetFileName(CTableContainer::TABLE_NORMAL_DROP,			"table_normal_drop_data");
	//fileNameList.SetFileName(CTableContainer::TABLE_SUPERIOR_DROP,			"table_superior_drop_data");
	//fileNameList.SetFileName(CTableContainer::TABLE_EXCELLENT_DROP,			"table_excellent_drop_data");
	//fileNameList.SetFileName(CTableContainer::TABLE_LEGENDARY_DROP,			"table_legendary_drop_data");
	//fileNameList.SetFileName(CTableContainer::TABLE_QUEST_DROP,				"table_quest_drop_data");
	//fileNameList.SetFileName(CTableContainer::TABLE_TYPE_DROP,				"table_type_drop_data");
	//fileNameList.SetFileName(CTableContainer::TABLE_EACH_DROP,				"table_each_drop_data");
	//fileNameList.SetFileName(CTableContainer::TABLE_SYSTEM_EFFECT,			"table_system_effect_data");


	/*flagManager.Set(CTableContainer::TABLE_GAME_MANIA_TIME);
	flagManager.Set(CTableContainer::TABLE_BASIC_DROP);
	flagManager.Set(CTableContainer::TABLE_LEGENDARY_DROP);
	flagManager.Set(CTableContainer::TABLE_NORMAL_DROP);
	flagManager.Set(CTableContainer::TABLE_SUPERIOR_DROP);
	flagManager.Set(CTableContainer::TABLE_EXCELLENT_DROP);*/

	g_pTableContainer = new CTableContainer;
	std::string str;
	CTable::eLOADING_METHOD eLoadMethod = (CTable::eLOADING_METHOD)byLoadMethod;
	str = "data";
	bool bResult = FALSE;
	bResult = g_pTableContainer->Create(flagManager, (char*)str.c_str(), &fileNameList, eLoadMethod, GetACP(), NULL);
	//g_pTableContainer->SaveToFile(flagManager, &fileNameList, false); 
	if (bResult != false)
	{
		//Print size of structs
		gs->printOk("==== LOADING GAMETABLES COMPLETE ====");
		//gs->printOk("==== LOADING MOBS / NPC ... ====");
		//mob->Create();
		//gs->printOk("==== LOADING MOBS / NPC COMPLETE ====");
	}
	else
		gs->printError("Failed to load tables");

	delete gs;
	return bResult;
}
void	CleanDatabase()
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();
	MySQLConnWrapper *db = new MySQLConnWrapper;
	db->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
	try
	{
		db->connect();
		printf("Connected to database server.\n\r");
	}
	catch (exception e)
	{
		printf("couldn't connect to database server ErrID:%s\n\r", e.what());
	}
	try
	{
		db->switchDb(app->GetConfigFileDatabase());
	}
	catch (exception e)
	{
		printf("Couldn't switch database to %s Error:%s\n\r", app->GetConfigFileDatabase(), e.what());
	}
	MySQLConnWrapper *db2 = new MySQLConnWrapper;
	db2->setConfig(app->GetConfigFileHost(), app->GetConfigFileUser(), app->GetConfigFilePassword(), app->GetConfigFileDatabase());
	try
	{
		db2->connect();
		//printf("Connected to database server.\n\r");
	}
	catch (exception e)
	{
		printf("couldn't connect to database server ErrID:%s\n\r", e.what());
	}
	try
	{
		db2->switchDb(app->GetConfigFileDatabase());
	}
	catch (exception e)
	{
		//printf("Couldn't switch database to %s Error:%s\n\r", app->GetConfigFileDatabase(), e.what());
	}

	db->prepare("SELECT * FROM characters");
	db->execute();
	while (db->fetch())
	{
		db2->prepare("UPDATE characters SET OnlineID = ? , isOnline=? WHERE CharID = ?");
		db2->setInt(1, 0);
		db2->setInt(2, 0);
		db2->setInt(3, db->getInt("CharID"));
		db2->execute();
	}
	GsFunctionsClass *gs = new GsFunctionsClass();
	gs->printOk("==== ONLINEID / ISONLINE CLEARED ==== ");
	delete gs, db, db2;
}

//-----------------------------------------------------------------------------------
//		GameServerMain
//-----------------------------------------------------------------------------------
int GameServerMain(int argc, _TCHAR* argv[])
{
	CGameServer app;
	CNtlFileStream traceFileStreams;
	CPacketTrace packetTrace;

// LOG FILE
	int rc = traceFileStreams.Create( "gamelog" );
	if( NTL_SUCCESS != rc )
	{
		printf( "log file CreateFile error %d(%s)", rc, NtlGetErrorMessage( rc ) );
		return rc;
	}

// CHECK INI FILE AND START PROGRAM
	NtlSetPrintStream( traceFileStreams.GetFilePtr() );
	NtlSetPrintFlag( PRINT_APP | PRINT_SYSTEM );


	rc = app.Create(argc, argv, ".\\Server.ini");
	if( NTL_SUCCESS != rc )
	{
		NTL_PRINT(PRINT_APP, "Server Application Create Fail %d(%s)", rc, NtlGetErrorMessage(rc) );
		return rc;
	}
	

	// CONNECT TO MYSQL
	app.db = new MySQLConnWrapper;
	app.db->setConfig(app.GetConfigFileHost(), app.GetConfigFileUser(), app.GetConfigFilePassword(), app.GetConfigFileDatabase());
	try
	{
		app.db->connect();
		printf("Connected to database server.");
	}
	catch (exception e)
	{
		printf("couldn't connect to database server ErrID:%s", e.what());
	}
	try
	{
		app.db->switchDb(app.GetConfigFileDatabase());
	}
	catch (exception e)
	{
		printf("Couldn't switch database to %s Error:%s", app.GetConfigFileDatabase(), e.what());
	}
	CleanDatabase();
	// NEW CLASS
	app.mob = new MobActivity();
	app.mob->app = &app;
	app.Start();
	Sleep(500);
	std::cout << "\n\n" << std::endl;
	std::cout << "\t  ____                              ____        _ _ " << std::endl;
	std::cout << "\t |  _ \\ _ __ __ _  __ _  ___  _ __ | __ )  __ _| | |" << std::endl;
	std::cout << "\t | | | | '__/ _` |/ _` |/ _ \\| '_ \\|  _ \\ / _` | | |" << std::endl;
	std::cout << "\t | |_| | | | (_| | (_| | (_) | | | | |_) | (_| | | |" << std::endl;
	std::cout << "\t |____/|_|  \\__,_|\\__, |\\___/|_| |_|____/ \\__,_|_|_|" << std::endl;
	std::cout << "\t                  |___/                             " << std::endl;
	std::cout << "\t______           Dragon Ball Tw Emulator	______\n\n" << std::endl;
	std::cout << "		Contributors - AKCORE, Xanu, Nicolas321				" << std::endl;
	//printf("<Ctrl-C> to stop.\n");
	CClientSession client;
	client.gsf = new GsFunctionsClass();
	std::string sCommand = "";
	std::wstring sMsg = L"";
	cout << "TWAlpha> ";
	while (true)
	{
		getline(cin, sCommand);
		if (!sCommand.empty())
			client.gsf->consoleCommandHandler(sCommand);
		else
			client.gsf->commandFinished();
	}
	app.WaitForTerminate();

	return 0;
}
