#pragma once

#include "NtlPacketCommon.h"

#include "Vector.h"
#include "Skill.h"
#include "NtlItem.h"
#include "NtlParty.h"
#include "Quest.h"
#include "NtlMail.h"
#include "Character.h"
#include "Guild.h"
#include "NtlPartyDungeon.h"
#include "NtlDojo.h"


enum eOPCODE_UG
{
	UG_OPCODE_BEGIN = 4000,

	UG_TUTORIAL_HINT_UPDATE_REQ,
	UG_TUTORIAL_WAIT_CANCEL_REQ,
	UG_TUTORIAL_PLAY_QUIT_REQ,
	UG_CHAR_DASH_KEYBOARD,
	UG_CHAR_DASH_MOUSE,
	UG_CHAR_JUMP,
	UG_CHAR_JUMP_END,
	UG_CHAR_FALLING,
	UG_CHAR_TOGG_SITDOWN,
	UG_CHAR_TOGG_FIGHTING,
	UG_CHAR_ATTACK_BEGIN,
	UG_CHAR_ATTACK_END,
	UG_CHAR_CHARGE,
	UG_CHAR_BLOCK_MODE,
	UG_CHAR_TELEPORT_REQ,
	UG_CHAR_BIND_REQ,
	UG_CHAR_REVIVAL_REQ,
	UG_CHAR_SERVER_CHANGE_REQ,
	UG_CHAR_CHANNEL_CHANGE_REQ,
	UG_CHAR_TARGET_SELECT,
	UG_CHAR_TARGET_INFO,
	UG_CHAR_TARGET_FACING,
	UG_CHAR_READY_FOR_COMMUNITY_SERVER_NFY,
	UG_CHAR_READY_TO_SPAWN,
	UG_CHAR_READY,
	UG_CHAR_MOVE,
	UG_CHAR_DEST_MOVE,
	UG_CHAR_CHANGE_HEADING,
	UG_CHAR_MOVE_COLLISION,
	UG_CHAR_MOVE_COLLISION_END,
	UG_CHAR_STANDING_SLIP,
	UG_CHAR_STANDING_SLIP_END,
	UG_CHAR_LOCATION_SYNC,
	UG_CHAR_AIR_MOVE_SYNC,
	UG_CHAR_CHANGE_DIRECTION_ON_FLOATING,
	UG_CHAR_FOLLOW_MOVE,
	UG_CHAR_FOLLOW_MOVE_SYNC,
	UG_GAME_ENTER_REQ,
	UG_GAME_LEAVE_REQ,
	UG_AUTH_KEY_FOR_COMMUNITY_SERVER_REQ,
	UG_ENTER_WORLD,
	UG_LOADING_COMPLETE_NFY,
	UG_SERVER_COMMAND,
	UG_PING,
	UG_SKILL_TARGET_LIST,
	UG_SKILL_LEARN_REQ,
	UG_SKILL_LEARN_BY_ITEM_REQ, 
	UG_SKILL_UPGRADE_REQ,
	UG_SKILL_RP_BONUS_SETTING_REQ,
	UG_HTB_START_REQ,
	UG_HTB_LEARN_REQ,
	UG_HTB_FORWARD_REQ,
	UG_HTB_RP_BALL_USE_REQ,
	UG_BUFF_DROP_REQ,
	UG_ITEM_MOVE_REQ,
	UG_ITEM_MOVE_STACK_REQ,
	UG_ITEM_DELETE_REQ,
	UG_ITEM_EQUIP_REPAIR_REQ,
	UG_ITEM_PICK_REQ,
	UG_ITEM_REPAIR_REQ,
	UG_ITEM_USE_REQ,
	UG_ITEM_UPGRADE_REQ,
	UG_ITEM_IDENTIFY_REQ,
	UG_ZENNY_PICK_REQ,
	UG_SHOP_START_REQ,
	UG_SHOP_BUY_REQ,
	UG_SHOP_SELL_REQ,
	UG_SHOP_END_REQ,
	UG_SHOP_SKILL_BUY_REQ,
	UG_PARTY_CREATE_REQ,
	UG_PARTY_DISBAND_REQ,
	UG_PARTY_INVITE_REQ,
	UG_PARTY_INVITE_CHARID_REQ,
	UG_PARTY_INVITE_CHAR_NAME_REQ,
	UG_PARTY_RESPONSE_INVITATION,
	UG_PARTY_LEAVE_REQ,
	UG_PARTY_KICK_OUT_REQ,
	UG_PARTY_CHANGE_LEADER_REQ,
	UG_PARTY_CHANGE_ZENNY_LOOTING_METHOD_REQ,
	UG_PARTY_CHANGE_ITEM_LOOTING_METHOD_REQ,
	UG_BANK_START_REQ,
	UG_BANK_MOVE_REQ,
	UG_BANK_MOVE_STACK_REQ,
	UG_BANK_END_REQ,
	UG_BANK_ZENNY_REQ,
	UG_BANK_BUY_REQ,
	UG_BANK_ITEM_DELETE_REQ,
	UG_SCOUTER_INDICATOR_REQ,
	UG_SCOUTER_PREDICT_REQ,
	UG_SCOUTER_EQUIP_CHECK_REQ,
	UG_SCOUTER_CHIP_REMOVE_ALL_REQ,
	UG_SCOUTER_ACTIVATION_REQ,
	UG_SOCIAL_ACTION,
	UG_TS_CONFIRM_STEP_REQ,
	UG_TS_CONFIRM_STEP_FOR_USE_ITEM_REQ,
	UG_TS_UPDATE_STATE,
	UG_TS_EXCUTE_TRIGGER_OBJECT,
	UG_QUEST_ITEM_MOVE_REQ,
	UG_QUEST_ITEM_DELETE_REQ,
	UG_QUEST_GIVEUP_REQ,
	UG_QUEST_SHARE,
	UG_QUEST_OBJECT_VISIT_REQ,
	UG_FREEBATTLE_CHALLENGE_REQ,
	UG_FREEBATTLE_ACCEPT_RES,
	UG_QUICK_SLOT_UPDATE_REQ,
	UG_QUICK_SLOT_DEL_REQ,
	UG_PET_DISMISS_PET_REQ,
	UG_TRADE_START_REQ,
	UG_TRADE_OK_RES,
	UG_TRADE_ADD_REQ,
	UG_TRADE_DEL_REQ,
	UG_TRADE_MODIFY_REQ,
	UG_TRADE_ZENNY_UPDATE_REQ,
	UG_TRADE_END_REQ,
	UG_TRADE_CANCEL_REQ,
	UG_TRADE_DENY_REQ,
	UG_GUILD_CREATE_REQ,
	UG_GUILD_INVITE_REQ,
	UG_DRAGONBALL_CHECK_REQ,
	UG_DRAGONBALL_REWARD_REQ,
	UG_TIMEQUEST_ENTER_REQ,
	UG_RANKBATTLE_INFO_REQ,
	UG_RANKBATTLE_JOIN_REQ,
	UG_RANKBATTLE_LEAVE_REQ,
	UG_PORTAL_ADD_REQ,
	UG_PORTAL_REQ,
	UG_PRIVATESHOP_CREATE_REQ,
	UG_PRIVATESHOP_EXIT_REQ,
	UG_PRIVATESHOP_OPEN_REQ,
	UG_PRIVATESHOP_CLOSE_REQ,
	UG_PRIVATESHOP_ITEM_UPDATE_REQ,
	UG_PRIVATESHOP_ITEM_INSERT_REQ,
	UG_PRIVATESHOP_ITEM_GET_REQ,
	UG_PRIVATESHOP_ITEM_DELETE_REQ,
	UG_PRIVATESHOP_ITEM_BUYING_REQ,
	UG_PRIVATESHOP_ENTER_REQ,
	UG_PRIVATESHOP_LEAVE_REQ,
	UG_PRIVATESHOP_ITEM_SELECT_REQ,
	UG_PRIVATESHOP_BUSINESS_REQUEST_REQ,
	UG_PRIVATESHOP_BUSINESS_CONSENT_RES,
	UG_PRIVATESHOP_BUSINESS_WAITTIMEOUT_NFY,
	UG_PRIVATESHOP_BUSINESS_PRICEFLUCTUATIONS_REQ,
	UG_PRIVATESHOP_BUSINESS_IMMEDIATE_ITEM_BUYING_REQ,
	UG_PRIVATESHOP_BUSINESS_CANCEL_REQ,
	UG_MAIL_START_REQ,
	UG_MAIL_SEND_REQ,
	UG_MAIL_READ_REQ,
	UG_MAIL_DEL_REQ,
	UG_MAIL_RETURN_REQ,
	UG_MAIL_RELOAD_REQ,
	UG_MAIL_LOAD_REQ,
	UG_MAIL_ITEM_RECEIVE_REQ,
	UG_MAIL_LOCK_REQ,
	UG_MAIL_MULTI_DEL_REQ,
	UG_WAR_FOG_UPDATE_REQ,
	UG_GUILD_FUNCTION_ADD_REQ,
	UG_GUILD_GIVE_ZENNY,
	UG_GUILD_BANK_START_REQ,
	UG_GUILD_BANK_MOVE_REQ,
	UG_GUILD_BANK_MOVE_STACK_REQ,
	UG_GUILD_BANK_END_REQ,
	UG_GUILD_BANK_ZENNY_REQ,
	UG_GUILD_CREATE_MARK_REQ,
	UG_GUILD_CHANGE_MARK_REQ,
	UG_CROSSFIRE_REQ,
	UG_BANK_LOAD_REQ,
	UG_GUILD_CHANGE_NAME_REQ,
	UG_PARTY_SHARETARGET_REQ,
	UG_RIDE_ON_BUS_REQ,
	UG_RIDE_OFF_BUS_REQ,
	UG_TRANSFORM_CANCEL_REQ,
	UG_SHOP_ITEM_IDENTIFY_REQ,
	UG_DICE_ROLL_REQ,
	UG_BUDOKAI_JOIN_INFO_REQ,
	UG_BUDOKAI_JOIN_STATE_REQ,
	UG_BUDOKAI_MUDOSA_INFO_REQ,
	UG_BUDOKAI_MUDOSA_TELEPORT_REQ,
	UG_BUDOKAI_PARTY_MAKER_REQ,
	UG_BUDOKAI_SOCIAL_ACTION,
	UG_BUDOKAI_PRIZEWINNER_NAME_REQ,
	UG_BUDOKAI_GM_BUDOKAI_SERVER_ENTER_REQ,
	UG_BUDOKAI_GM_BUDOKAI_SERVER_LEAVE_REQ,
	UG_BUDOKAI_GM_MATCH_PROGRESS_INFO_REQ,
	UG_BUDOKAI_GM_MATCH_ARENA_ENTER_REQ,
	UG_BUDOKAI_GM_MATCH_ARENA_LEAVE_REQ,
	UG_BUDOKAI_JOIN_TEAM_INFO_REQ,
	UG_BUDOKAI_JOIN_TEAM_REQ,
	UG_BUDOKAI_LEAVE_TEAM_REQ,
	UG_BUDOKAI_LEAVE_TEAM_MEMBER_REQ,
	UG_BUDOKAI_JOIN_INDIVIDUAL_REQ,
	UG_BUDOKAI_LEAVE_INDIVIDUAL_REQ,
	UG_SCOUTER_TURN_ON,
	UG_SCOUTER_TURN_OFF,
	UG_SCOUTER_ITEM_SELL_REQ,
	UG_SHOP_EVENTITEM_START_REQ,
	UG_SHOP_EVENTITEM_BUY_REQ,
	UG_SHOP_EVENTITEM_END_REQ,
	UG_SHOP_GAMBLE_BUY_REQ,
	UG_PARTY_DIFF_CHANGE_REQ,
	UG_PARTY_DUNGEON_INIT_REQ,
	UG_VEHICLE_DIRECT_PLAY_CANCEL_NFY,
	UG_VEHICLE_ENGINE_START_REQ,
	UG_VEHICLE_ENGINE_STOP_REQ,
	UG_VEHICLE_END_REQ,
	UG_VEHICLE_STUNT_NFY,
	UG_VEHICLE_FUEL_REMOVE_NFY,
	UG_SKILL_INIT_REQ,
	UG_SKILL_RESET_PLUS_REQ,
	UG_TELEPORT_CONFIRM_REQ,
	UG_HOIPOIMIX_ITEM_CHECK_REQ,
	UG_HOIPOIMIX_ITEM_MAKE_REQ,
	UG_HOIPOIMIX_ITEM_MAKE_EP_REQ,
	UG_HOIPOIMIX_ITEM_MACHINE_DEL_REQ,
	UG_HOIPOIMIX_JOB_SET_REQ,
	UG_HOIPOIMIX_JOB_RESET_REQ,
	UG_HOIPOIMIX_ITEM_CREATE_EX_REQ,
	UG_ITEM_DISASSEMBLE_REQ,
	UG_CASHITEM_START_REQ,
	UG_CASHITEM_END_REQ,
	UG_CASHITEM_MOVE_REQ,
	UG_CASHITEM_USE_REQ,
	UG_CASHITEM_HLSHOP_START_REQ,
	UG_CASHITEM_HLSHOP_END_REQ,
	UG_CASHITEM_HLSHOP_REFRESH_REQ,
	UG_CASHITEM_BUY_REQ,
	UG_CASHITEM_SEND_GIFT_REQ,
	UG_DOJO_CREATE_REQ,
	UG_DOJO_DELETE_REQ,
	UG_DOJO_FUNCTION_ADD_REQ,
	UG_DOJO_BANK_HISTORY_REQ,
	UG_DOGI_CREATE_REQ,
	UG_DOGI_CHANGE_REQ,
	UG_GUILD_DOGI_CREATE_REQ,
	UG_GUILD_DOGI_CHANGE_REQ,
	UG_DOJO_SCRAMBLE_REQ,
	UG_DOJO_SCRAMBLE_RESPONSE_REQ,
	UG_SHOP_NETPYITEM_START_REQ,
	UG_SHOP_NETPYITEM_BUY_REQ,
	UG_SHOP_NETPYITEM_END_REQ,
	UG_WORLD_MAP_STATUS,
	UG_DOJO_NPC_INFO_REQ,
	UG_GAMEGUARD_AUTH_ANSWER_RES,
	UG_GAMEGUARD_HACK_INFO_NFY,
	UG_SKILL_CASTING_CANCELED_NFY,
	UG_PET_ATTACK_TARGET_NFY,
	UG_DURATION_ITEM_BUY_REQ,
	UG_DURATION_RENEW_REQ,
	UG_BUDOKAI_PRIZEWINNER_TEAM_NAME_REQ,
	UG_GMT_UPDATE_REQ,
	UG_CHAR_LOC_AFTER_KNOCKDOWN_NFY,
	UG_CHAR_LOC_AFTER_SLIDING_NFY,
	UG_CHAR_LOC_AFTER_PUSH_NFY,
	UG_SCS_CHECK_START_RES,
	UG_SCS_CHECK_RES,
	UG_SCS_REPLY_REQ,
	UG_SCS_REMAKE_REQ,
	UG_QUICK_TELEPORT_LOAD_REQ,
	UG_QUICK_TELEPORT_UPDATE_REQ,
	UG_QUICK_TELEPORT_DEL_REQ,
	UG_QUICK_TELEPORT_USE_REQ,
	UG_CHAR_CRESCENT_POPO_REVIVAL_REQ,
	UG_CHARACTER_RENAME_REQ,
	UG_CASHITEM_UPDATE_DURATION_REQ,
	UG_CHAR_DEST_MOVE_SYNC,
	UG_CHAR_NPCSERVER_MOVE_SYNC,
	UG_HOIPOIMIX_ITEM_CREATE_REQ,
	UG_CHAR_EXIT_REQ,
	UG_GAME_EXIT_REQ,
	UG_CHAR_AWAY_REQ,
	UG_CHAR_KEY_UPDATE_REQ,
	UG_CHAR_DIRECT_PLAY_CANCEL,
	UG_CHAR_DIRECT_PLAY_ACK,
	UG_CHAR_KNOCKDOWN_RELEASE_NFY,
	UG_CHAR_SKILL_REQ,
	UG_DRAGONBALL_SCRAMBLE_JOIN_REQ,
	UG_DRAGONBALL_SCRAMBLE_BALL_LOC_REQ,
	UG_CHAR_SKILL_INIT_REQ,
	UG_GUILD_DOGI_DYE_REQ,
	UG_TENKAICHIDAISIJYOU_SELL_REQ,
	UG_TENKAICHIDAISIJYOU_SELL_CANCEL_REQ,
	UG_TENKAICHIDAISIJYOU_BUY_REQ,
	UG_TENKAICHIDAISIJYOU_LIST_REQ,
	UG_MASCOT_SUMMON_REQ,
	UG_MASCOT_UNSUMMON_REQ,
	UG_MASCOT_DELETE_REQ,
	UG_MASCOT_RING_ACTION_REQ,
	UG_MASCOT_RING_REMOVE_ALL_REQ,
	UG_MASCOT_SUMMON_REQ_EX,
	UG_MASCOT_UNSUMMON_REQ_EX,
	UG_MASCOT_DELETE_REQ_EX,
	UG_MASCOT_RING_MATERIAL_DISASSEMBLE_REQ,
	UG_MASCOT_RING_ACTION_END_REQ,
	UG_REMOTE_SHOP_BUY_REQ,
	UG_REMOTE_SHOP_SELL_REQ,
	UG_REMOTE_ITEM_EQUIP_REPAIR_REQ,
	UG_REMOTE_ITEM_REPAIR_REQ,
	UG_REMOTE_SHOP_ITEM_IDENTIFY_REQ,
	UG_WORLD_LAVA_ENTERED_NFY,
	UG_WORLD_LAVA_LEFT_NFY,
	UG_SKILL_ONE_RESET_REQ,
	UG_CASHITEM_INFO_REQ,
	UG_ULTIMATE_DUNGEON_ENTER_REQ,
	UG_PARTY_MATCHING_REGISTER_REQ,
	UG_PARTY_MATCHING_JOIN_REQ,
	UG_PARTY_MATCHING_ROLEPLAY_REQ,
	UG_PARTY_MATCHING_UNREGISTER_REQ,
	UG_PARTY_MATCHING_LIST_REQ,
	UG_PARTY_MATCHING_INFO_REQ,
	UG_PARTY_MATCHING_ENTER_DUNGEON_REQ,
	UG_PARTY_MATCHING_ENTER_DUNGEON_AGREE_NFY,
	UG_DWC_WORLDCOUNT_INFO_REQ,
	UG_DWC_ENTER_REQ,
	UG_DWC_ENTER_CONFIRM_REQ,
	UG_DWC_SCENARIO_INFO_REQ,
	UG_DWC_GET_REWARD_REQ,
	UG_CHARTITLE_SELECT_REQ,
	UG_TMQ_LIMIT_COUNT_PLUS_REQ,
	UG_PARTY_SELECT_STATE_REQ,
	UG_BATTLE_DUNGEON_ENTER_REQ,
	UG_BATTLE_DUNGEON_LEAVE_REQ,
	UG_ITEM_UPGRADE_WORK_REQ,
	UG_ITEM_UPGRADE_PROBABLEITEM_REQ,
	UG_REQUEST_CHAR_HISTORY_DATA_REQ,
	UG_EVENT_REWARD_LOAD_REQ,
	UG_EVENT_REWARD_SELECT_REQ,
	UG_DROPITEM_INFO_REQ,
	UG_ITEM_UPGRADE_BY_COUPON_REQ,
	UG_VEHICLE_FUEL_INSERT_REQ,
	UG_ITEM_EXCHANGE_REQ,
	UG_GAMEGUARD3_AUTH_ANSWER_RES,
	UG_ITEM_UPGRADED_EXCHANGE_REQ,
	UG_ITEM_SEAL_REQ,
	UG_ITEM_SEAL_EXTRACT_REQ,
	UG_PERFORMANCE_DATA_NFY,
	UG_PERFORMANCE_DATA_2_NFY,
	UG_PARTY_CHANGE_ITEM_LOOTING_RANK_METHOD_REQ,
	UG_ITEM_DICE_REQ,
	UG_GIFT_SHOP_START_REQ,
	UG_GIFT_SHOP_BUY_REQ,
	UG_ITEM_SOCKET_INSERT_BEAD_REQ,
	UG_ITEM_SOCKET_DESTROY_BEAD_REQ,
	UG_ITEM_CHAGE_BATTLE_ATTRIBUTE_REQ,
	UG_INVISIBLE_COSTUME_UPDATE_REQ,
	UG_INVENTORY_SORT_REQ,
	UG_ITEM_CHANGE_OPTION_REQ,
	UG_DYNAMIC_FIELD_SYSTEM_BOSS_POSITION_REQ,
	UG_CHAR_AIR_MOVE,
	UG_CHAR_AIR_JUMP,
	UG_CHAR_AIR_DASH,
	UG_CHAR_AIR_FALLING,
	UG_CHAR_AIR_END,
	UG_CHAR_AIR_FOLLOW_MOVE,
	UG_CHAR_AIR_ACCEL,
	UG_CHAR_AIR_POSE_REQ,
	UG_PET_ATTACK_TOGGLE_NFY,
	UG_PET_SKILL_TOGGLE_NFY,
	UG_PET_RECALL_NFY,
	UG_MASCOT_FUSION_REQ,
	UG_MASCOT_SKILL_ADD_REQ,
	UG_MASCOT_SKILL_UPDATE_REQ,
	UG_MASCOT_SKILL_UPGRADE_REQ,
	UG_MASCOT_SKILL_REQ,
	UG_MASCOT_AUTO_LOOTING_REQ,
	UG_MASCOT_REMOTE_SHOP_SELL_REQ,
	UG_MASCOT_SEAL_SET_REQ,
	UG_MASCOT_SEAL_CLEAR_REQ,	
	UG_PORTAL_START_REQ,
	
	//UG_PING, = UG_OPCODE_BEGIN,
	/*UG_GAME_ENTER_REQ,
	UG_GAME_LEAVE_REQ,									// °ÔÀÓ ³ª°¨
	UG_AUTH_KEY_FOR_COMMUNITY_SERVER_REQ,				// Ä¿¹Â´ÏÆ¼ ¼­¹ö ÀÎÁõÅ° ¿äÃ»
	UG_ENTER_WORLD,										// ¿ùµå ÁøÀÔ
	UG_SERVER_COMMAND,									// ¼­¹ö ¸í·É¾î

	UG_CHAR_READY_FOR_COMMUNITY_SERVER_NFY,				// Ä¿¹Â´ÏÆ¼ ¼­¹ö¿ÍÀÇ µ¥ÀÌÅ¸ Àü¼Û ÁØºñ°¡ µÇ¾úÀ½À» ¾Ë¸²
	UG_CHAR_READY_TO_SPAWN,								// ·Îµù ¿Ï·á(Spawning state·ÎÀÇ ÁøÀÔ ÁØºñ »óÅÂ)
	UG_CHAR_READY,										// Ä³¸¯ÅÍ ÁØºñ
	UG_CHAR_MOVE,										// Ä³¸¯ÅÍ ÀÌµ¿
	UG_CHAR_DEST_MOVE,									// Ä³¸¯ÅÍ ¸ñÀûÁö ÀÌµ¿
	UG_CHAR_CHANGE_HEADING,								// Ä³¸¯ÅÍ È¸Àü
	UG_CHAR_MOVE_SYNC,									// Ä³¸¯ÅÍ À§Ä¡ º¸Á¤
	UG_CHAR_CHANGE_DIRECTION_ON_FLOATING,				// Ä³¸¯ÅÍ Á¡ÇÁ/falling ¶§ÀÇ ¹Ù¶óº¸´Â ¹æÇâ
	UG_CHAR_FOLLOW_MOVE,								// Ä³¸¯ÅÍ Å¸°Ù µû¶ó°¡±â
	UG_CHAR_DASH_KEYBOARD,								// Keyboard dash ½ÃÀÛ
	UG_CHAR_DASH_MOUSE,									// Mouse dash ½ÃÀÛ
	UG_CHAR_JUMP,										// Á¡ÇÁ ½ÃÀÛ
	UG_CHAR_JUMP_END,									// Á¡ÇÁ Á¾·á
	UG_CHAR_FALLING,									// Falling »óÅÂ ½ÃÀÛ/Á¾·á
	UG_CHAR_TOGG_SITDOWN,								// ¾É±â¸ðµå Åä±Û
	UG_CHAR_TOGG_FIGHTING,								// ÀüÅõ¸ðµå Åä±Û
	UG_CHAR_TOGG_RUNNING,								// ¶Ù±â¸ðµå Åä±Û ( ÇöÀç Å¬¶óÀÌ¾ðÆ®´Â »ç¿ëÇÏÁö ¾ÊÀ½ )
	UG_CHAR_TARGET_SELECT,								// Å¸°Ù ¼±ÅÃ
	UG_CHAR_TARGET_INFO,								// Å¸°Ù Á¤º¸ ¿äÃ»
	UG_CHAR_TARGET_FACING,								// Å¸°Ù ´ë¸é
	UG_CHAR_ATTACK_BEGIN,								// °ø°Ý ½ÃÀÛ ¾Ë¸²
	UG_CHAR_ATTACK_END,									// °ø°Ý Á¾·á ¾Ë¸²
	UG_CHAR_CHARGE,										// Ä³¸¯ÅÍ ±âÃæÀü
	UG_CHAR_BLOCK_MODE,									// Block ¸ðµå ½ÃÀÛ/Á¾·á
	UG_CHAR_TELEPORT_REQ,								// ÅÚ·¹Æ÷Æ® ¿äÃ»
	UG_CHAR_BIND_REQ,									// ¹ÙÀÎµå ¿äÃ»
	UG_CHAR_REVIVAL_REQ,								// ºÎÈ° ¿äÃ»
	UG_CHAR_SERVER_CHANGE_REQ,							// ´Ù¸¥ °ÔÀÓ ¼­¹ö·ÎÀÇ ÀÌµ¿ ¿äÃ»
	UG_CHAR_CHANNEL_CHANGE_REQ,							// Ä³¸¯ÅÍ ³ª°¨ -> ´Ù¸¥ ¼­¹ö Ã¤³Î·Î ÀÌµ¿ ¿äÃ»
	UG_CHAR_EXIT_REQ,									// Ä³¸¯ÅÍ ³ª°¨ -> Ä³¸¯ÅÍ ¼­¹ö·Î ÀÌµ¿ ¿äÃ»
	UG_GAME_EXIT_REQ,									// °ÔÀÓÀ» Á¾·áÇÔ
	UG_CHAR_AWAY_REQ,
	UG_CHAR_KEY_UPDATE_REQ,
	UG_CHAR_DIRECT_PLAY_ACK,							// ¿¬Ãâ Á¾·áÈÄ º¸³»ÁÖ´Â ÀÀ´ä
	UG_CHAR_KNOCKDOWN_RELEASE_NFY,						// ³Ë´Ù¿î Ç®±â
	UG_CHAR_SKILL_REQ,									// ½ºÅ³ »ç¿ë ¿äÃ»

	UG_SKILL_TARGET_LIST,								// ½ºÅ³ Å¸°Ù ¸ñ·Ï ¾Ë¸²
	UG_SKILL_LEARN_REQ,									// ½ºÅ³ ¼ö·Ã ¿äÃ»
	UG_SKILL_UPGRADE_REQ,								// ½ºÅ³ Çâ»ó ¿äÃ»
	UG_SKILL_RP_BONUS_SETTING_REQ,						// ½ºÅ³¸¶´Ù ÀÚµ¿À¸·Î »ç¿ëµÉ RP º¸³Ê½º Å¸ÀÔ ¼³Á¤
	UG_HTB_START_REQ,								    // HTB ½ÃÀÛ ¿äÃ»
	UG_HTB_LEARN_REQ,									// HTB ½Àµæ ¿äÃ»
	UG_HTB_FORWARD_REQ,									// HTB ÁøÇà
	UG_HTB_RP_BALL_USE_REQ,

	UG_BUFF_DROP_REQ,									// Buff ÇØÁ¦ ¿äÃ»

	UG_ITEM_MOVE_REQ,									// ¾ÆÀÌÅÛ ÀÌµ¿ ¿äÃ»
	UG_ITEM_MOVE_STACK_REQ,								// ¾ÆÀÌÅÛ ½ºÅÃ ÀÌµ¿ ¿äÃ»
	UG_ITEM_DELETE_REQ,									// ¾ÆÀÌÅÛ »èÁ¦ ¿äÃ»
	UG_ITEM_EQUIP_REPAIR_REQ,							// ¼ö¸® ½ÃÀÛ ¿äÃ»
	UG_ITEM_PICK_REQ,									// ¾ÆÀÌÅÛÁÝ±â ¿äÃ»
	UG_ITEM_REPAIR_REQ,
	UG_ITEM_USE_REQ,									// ¾ÆÀÌÅÛ »ç¿ë ¿äÃ»
	UG_ITEM_UPGRADE_REQ,								// ¾ÆÀÌÅÛ ¾÷±×·¹ÀÌµå
	UG_ITEM_IDENTIFY_REQ,								// ½ºÄ«¿ìÅÍÆÄÃ÷·Î ¾ÆÀÌÅÛ È®ÀÎ

	UG_ZENNY_PICK_REQ,									// µ·ÁÝ±â ¿äÃ»

	UG_SHOP_START_REQ,									// »óÁ¡ ¿­±â
	UG_SHOP_BUY_REQ,									// »óÁ¡ ¹°°Ç »ç±â
	UG_SHOP_SELL_REQ,									// »óÁ¡ ¹°°Ç ÆÈ±â
	UG_SHOP_END_REQ,									// »óÁ¡ ´Ý±â
	UG_SHOP_SKILL_BUY_REQ,								// ½ºÅ³ ±¸ÀÔ ¿äÃ»

	UG_PARTY_CREATE_REQ,								// ÆÄÆ¼ ¸¸µé±â
	UG_PARTY_DISBAND_REQ,								// ÆÄÆ¼ ÇØ»êÇÏ±â
	UG_PARTY_INVITE_REQ,								// ÆÄÆ¼¿¡ ÃÊ´ëÇÏ±â
	UG_PARTY_INVITE_CHARID_REQ,							// ÆÄÆ¼¿¡ ÃÊ´ëÇÏ±â(»ó´ë¹æÀÇ CharID ÀÌ¿ë)
	UG_PARTY_INVITE_CHAR_NAME_REQ,						// ÆÄÆ¼¿¡ ÃÊ´ëÇÏ±â(»ó´ë¹æÀÇ ÀÌ¸§ ÀÌ¿ë)
	UG_PARTY_RESPONSE_INVITATION,						// ÆÄÆ¼ ÃÊ´ë¿¡ ´ëÇÑ ´ë´ä º¸³»±â
	UG_PARTY_LEAVE_REQ,									// ÆÄÆ¼ Å»ÅðÇÏ±â
	UG_PARTY_KICK_OUT_REQ,								// ¸â¹ö °­Á¦·Î Å»Åð½ÃÅ°±â
	UG_PARTY_CHANGE_LEADER_REQ,							// ÆÄÆ¼ ¸®´õ º¯°æÇÏ±â
	UG_PARTY_OPEN_CHARM_SLOT_REQ,						// ÆÄÆ¼ ºÎÀû µî·ÏÃ¢ ¿­±â
	UG_PARTY_CLOSE_CHARM_SLOT_REQ,						// ÆÄÆ¼ ºÎÀû µî·ÏÃ¢ ´Ý±â
	UG_PARTY_REGISTER_VICTIM_ITEM_REQ,					// ºÎÀû Á¦¹°(¾ÆÀÌÅÛ) µî·ÏÇÏ±â
	UG_PARTY_UNREGISTER_VICTIM_ITEM_REQ,				// µî·ÏµÈ ºÎÀû Á¦¹°(¾ÆÀÌÅÛ) ´Ù½Ã °¡Á®¿À±â

	UG_PARTY_REGISTER_VICTIM_ZENNY_REQ,					// ºÎÀû Á¦¹°(Á¦´Ï) µî·ÏÇÏ±â
	UG_PARTY_UNREGISTER_VICTIM_ZENNY_REQ,				// µî·ÏµÈ ºÎÀû Á¦¹°(Á¦´Ï) ´Ù½Ã °¡Á®¿À±â
	UG_PARTY_DECLARE_ZENNY_REQ,							// Á¦´Ï µî·ÏÇÏ±â
	UG_PARTY_ACTIVATE_CHARM_REQ,						// ºÎÀû È°¼ºÈ­ÇÏ±â
	UG_PARTY_DEACTIVATE_CHARM_REQ,						// È°¼ºÈ­µÈ ºÎÀû È¿°ú Á¦°ÅÇÏ±â
	UG_PARTY_CHANGE_ZENNY_LOOTING_METHOD_REQ,			// Á¦´Ï ºÐ¹è ¹æ¹ý º¯°æÇÏ±â
	UG_PARTY_CHANGE_ITEM_LOOTING_METHOD_REQ,			// ¾ÆÀÌÅÛ ºÐ¹è ¹æ¹ý º¯°æÇÏ±â
	UG_PARTY_INVEST_ZENNY_REQ,							// Á¦´Ï¸¦ ÆÄÆ¼ ÀÎº¥Åä¸®ÀÇ ¾ÆÀÌÅÛ¿¡ ÅõÀÚÇÏ±â
	UG_PARTY_INVEN_ITEM_RANK_SET_REQ,					// ÆÄÆ¼ÀÎº¥¿¡ ³ÖÀ» ¾ÆÀÌÅÛÀÇ RANK¼³Á¤ÇÏ±â

	UG_BANK_START_REQ,									// Warehouse ¾ÆÀÌÅÛ ·Îµù
	UG_BANK_MOVE_REQ,									// Warehouse ÀÇ ¾ÆÀÌÅÛ ÀÌµ¿
	UG_BANK_MOVE_STACK_REQ,								// Warehouse ÀÇ ½ºÅÃ¾ÆÀÌÅÛ ÀÌµ¿
	UG_BANK_END_REQ,									// Warehouse ¸¦ ´Ý´Â´Ù.
	UG_BANK_ZENNY_REQ,									// Warehouse ¿¡ Àú±Ý È¤Àº ÀÎÃâÇÑ´Ù.
	UG_BANK_BUY_REQ,									// Warehouse ÀÇ Ã¢°í°¡¹æÀ» ´Ã¸°´Ù.
	UG_BANK_ITEM_DELETE_REQ,							// Warehouse ÀÇ ¾ÆÀÌÅÛÀ» »èÁ¦ÇÑ´Ù.

	UG_SCOUTER_INDICATOR_REQ,							// ½ºÄ«¿ìÅÍ ÀüÅõ·ÂÃøÁ¤	
	UG_SCOUTER_PREDICT_REQ,								// ½ºÄ«¿ìÅÍ ¾÷±×·¹ÀÌµå ¿¹Ãø	
	UG_SCOUTER_EQUIP_CHECK_REQ,							// ½ºÄ«¿ìÅÍ »ó´ë ÀåÂø¾ÆÀÌÅÛ È®ÀÎ
	UG_SCOUTER_CHIP_REMOVE_ALL_REQ,						// ½ºÄ«¿ìÅÍ Ä¨À» ¸ðµÎ Á¦°Å

	UG_SOCIAL_ACTION,									// Social Action »ç¿ë

	UG_TS_CONFIRM_STEP_REQ,								// Æ®¸®°Å ÁøÇà ¿äÃ»
	UG_TS_UPDATE_STATE,									// Æ®¸®°Å ½ÇÆÐ »óÅÂ ¾÷µ¥ÀÌÆ®
	UG_TS_EXCUTE_TRIGGER_OBJECT,						// Excute Trigger Object

	UG_QUEST_ITEM_MOVE_REQ,								// Äù½ºÆ® ¾ÆÀÌÅÛ ÀÌµ¿ ¿äÃ»
	UG_QUEST_ITEM_DELETE_REQ,							// Äù½ºÆ® ¾ÆÀÌÅÛ »èÁ¦ ¿äÃ»
	UG_QUEST_GIVEUP_REQ,								// Äù½ºÆ® ¾ÆÀÌÅÛ Æ÷±â ¿äÃ»
	UG_QUEST_SHARE,										// Äù½ºÆ® °øÀ¯ ¿äÃ»
	UG_QUEST_OBJECT_VISIT_REQ,							// npc, tobject ¹æ¹® ÀÌº¥Æ®

	UG_FREEBATTLE_CHALLENGE_REQ,						// ÇÁ¸®¹èÆ² ´ëÀü ½ÅÃ»
	UG_FREEBATTLE_ACCEPT_RES,							// ÇÁ¸®¹èÆ² ´ëÀü ¼ö¶ôÀÀ´ä

	UG_QUICK_SLOT_UPDATE_REQ,							// ºü¸¥½½·Ô Ãß°¡ ÇÏ°Å³ª ¼öÁ¤
	UG_QUICK_SLOT_DEL_REQ,								// ºü¸¥½½·Ô »èÁ¦

	UG_PET_DISMISS_PET_REQ,								// ÆêÀÇ ¼ÒÈ¯ Ãë¼Ò ¿äÃ»

	UG_TRADE_START_REQ,									// ÇØ´ç À¯Àú¿ÍÀÇ ±³È¯ ¿äÃ»
	UG_TRADE_OK_RES,									// ÇØ´ç À¯Á®ÀÇ ÀÀ´ä
	UG_TRADE_ADD_REQ,                                   // ±³È¯Ã¢¿¡ ¾ÆÀÌÅÛ µî·Ï 
	UG_TRADE_DEL_REQ,									// ±³È¯Ã¢¿¡ ¾ÆÀÌÅÛ »èÁ¦
	UG_TRADE_MODIFY_REQ,                                   // ±³È¯Ã¢¿¡ µî·ÏµÈ ¾ÆÀÌÅÛ °³¼ö º¯°æ 
	UG_TRADE_ZENNY_UPDATE_REQ,							// ±³È¯Ã¢¿¡ Á¦´Ï µî·Ï ¹× º¯°æ
	UG_TRADE_END_REQ,									// ±³È¯ ¿Ï·á ¹öÆ° Å¬¸¯
	UG_TRADE_CANCEL_REQ,								// ±³È¯ Ãë¼Ò È¤Àº Ä³¸¯Á¾·á °Å¸® ÀÌÅ»½Ã »ç¿ë
	UG_TRADE_DENY_REQ,									// ±³È¯ÀÌ ºÒ°¡´É ÇÏ°Ô ÇÏ°Å³ª °¡´ÉÇÏµµ·Ï ÇÑ´Ù.

	UG_GUILD_CREATE_REQ,								// ±æµå ¸¸µé±â ( Guild Creation/Disbanding )
	UG_GUILD_INVITE_REQ,								// ±æµå¿¡ ÃÊ´ëÇÏ±â ( Guild Member Invitation/Leaving/Kicking Out )

	UG_TUTORIAL_HINT_UPDATE_REQ,						// ÈùÆ® ÁøÇà »óÈ²À» ¾÷µ¥ÀÌÆ®ÇÑ´Ù.
	UG_TUTORIAL_WAIT_CANCEL_REQ,						// Æ©Åä¸®¾ó ´ë±â Ãë¼ÒÇÏ±â
	UG_TUTORIAL_PLAY_QUIT_REQ,							// Æ©Åä¸®¾ó ÇÃ·¹ÀÌ Ãë¼ÒÇÏ±â

	UG_TIMEQUEST_ROOM_LIST_REQ,							// Å¸ÀÓ¸Ó½Å Äù½ºÆ® ¹æ ¸ñ·Ï ¿äÃ»
	UG_TIMEQUEST_ROOM_INFO_REQ,							// Å¸ÀÓ¸Ó½Å Äù½ºÆ® ¹æ Á¤º¸ ¿äÃ»
	UG_TIMEQUEST_ROOM_JOIN_REQ,							// Å¸ÀÓ¸Ó½Å Äù½ºÆ® ¹æ ÁøÀÔ ¿äÃ»
	UG_TIMEQUEST_ROOM_LEAVE_REQ,						// Å¸ÀÓ¸Ó½Å Äù½ºÆ® ¹æ ÅðÀå ¿äÃ»
	UG_TIMEQUEST_ROOM_TELEPORT_REQ,						// Å¸ÀÓ¸Ó½Å Äù½ºÆ® ½ÃÀÛ À§Ä¡·Î ÅÚ·¹Æ÷Æ®

	UG_RANKBATTLE_INFO_REQ,
	UG_RANKBATTLE_JOIN_REQ,
	UG_RANKBATTLE_LEAVE_REQ,

	UG_RANKBATTLE_ROOM_LIST_REQ,						// RankBattle ¹æ ¸ñ·Ï ¿äÃ»
	UG_RANKBATTLE_ROOM_INFO_REQ,						// RankBattle ¹æ Á¤º¸ ¿äÃ»
	UG_RANKBATTLE_ROOM_CREATE_REQ,						// RankBattle ¹æ »ý¼º ¿äÃ»
	UG_RANKBATTLE_ROOM_LEAVE_REQ,						// RankBattle ¹æ »èÁ¦ ¿äÃ»
	UG_RANKBATTLE_ROOM_CHALLENGE_REQ,					// RankBattle ´ë·Ã ¿äÃ»
	UG_RANKBATTLE_ROOM_MATCH_CANCEL_REQ,				// RankBattle ´ë·Ã ÁØºñ ¿Ï·á ¾Ë¸²ÀÌ ¿Â ÈÄ ´ë·ÃÀ» Ãë¼ÒÇÔ(°³ÀÎ)

	UG_DRAGONBALL_CHECK_REQ,							// µå·¡°ïº¼ UI 7°³ ¹èÄ¡OK
	UG_DRAGONBALL_REWARD_REQ,							// µå·¡°ïº¼ ´Ù ¸ðÀ½¿¡ ´ëÇÑ º¸»ó ¼±ÅÃ ¿äÃ»

	UG_PRIVATESHOP_CREATE_REQ,							// »óÁ¡À» ¸¸µé¾î¶ó ¿äÃ»[6/28/2007 SGpro]
	UG_PRIVATESHOP_EXIT_REQ,							// »óÁ¡À» ¿ÏÀüÈ÷ ´Ý¾Æ¶ó ¿äÃ»[6/28/2007 SGpro]
	UG_PRIVATESHOP_OPEN_REQ,							// »óÁ¡ »óÅÂ¸¦ OpenÀ¸·Î ÇØ¶ó ¿äÃ»[6/28/2007 SGpro]
	UG_PRIVATESHOP_CLOSE_REQ,							// »óÁ¡ »óÅÂ¸¦ Close·Î ÇØ¶ó ¿äÃ»[6/28/2007 SGpro]
	UG_PRIVATESHOP_ITEM_UPDATE_REQ,						// »óÁ¡ ¾ÆÀÌÅÛ¿¡ ³»¿ëÀ» º¯°æ ¿äÃ»[6/29/2007 SGpro]
	UG_PRIVATESHOP_ITEM_INSERT_REQ,						// »óÁ¡ ¾ÆÀÌÅÛ Ãß°¡ ¿äÃ»[6/29/2007 SGpro]
	UG_PRIVATESHOP_ITEM_GET_REQ,						// »óÁ¡ ¾ÆÀÌÅÛ Á¤º¸ ¿äÃ»
	UG_PRIVATESHOP_ITEM_DELETE_REQ,						// »óÁ¡ ¾ÆÀÌÅÛ »èÁ¦ ¿äÃ»[6/29/2007 SGpro]
	UG_PRIVATESHOP_ITEM_BUYING_REQ,						// »óÁ¡ ¾ÆÀÌÅÛ »ç±â ¿äÃ»[6/29/2007 SGpro]
	UG_PRIVATESHOP_ENTER_REQ,							// ¼Õ´Ô ÀÔÀå[6/29/2007 SGpro]
	UG_PRIVATESHOP_LEAVE_REQ,							// ¼Õ´Ô ÅðÀå[6/29/2007 SGpro]
	UG_PRIVATESHOP_ITEM_SELECT_REQ,						// ¾ÆÀÌÅÛ Âò[7/19/2007 SGpro]

	UG_MAIL_START_REQ,
	UG_MAIL_SEND_REQ,
	UG_MAIL_READ_REQ,
	UG_MAIL_DEL_REQ,
	UG_MAIL_RETURN_REQ,
	UG_MAIL_RELOAD_REQ,
	UG_MAIL_LOAD_REQ,
	UG_MAIL_ITEM_RECEIVE_REQ,
	UG_MAIL_LOCK_REQ,
	UG_MAIL_MULTI_DEL_REQ,

	UG_PRIVATESHOP_BUSINESS_REQUEST_REQ,				// [10/17/2007 SGpro]
	UG_PRIVATESHOP_BUSINESS_CONSENT_RES,				// [10/17/2007 SGpro]
	UG_PRIVATESHOP_BUSINESS_WAITTIMEOUT_NFY,			// [10/17/2007 SGpro]
	UG_PRIVATESHOP_BUSINESS_PRICEFLUCTUATIONS_REQ,		// [10/17/2007 SGpro]
	UG_PRIVATESHOP_BUSINESS_IMMEDIATE_ITEM_BUYING_REQ,	// [10/17/2007 SGpro]
	UG_PRIVATESHOP_BUSINESS_CANCEL_REQ,					// [10/17/2007 SGpro]

	UG_PORTAL_START_REQ,
	UG_PORTAL_ADD_REQ,
	UG_PORTAL_REQ,

	UG_WAR_FOG_UPDATE_REQ,	
	UG_GUILD_FUNCTION_ADD_REQ,
	UG_GUILD_GIVE_ZENNY,

	UG_GUILD_BANK_START_REQ,							// GUILD Warehouse ¾ÆÀÌÅÛ ·Îµù
	UG_GUILD_BANK_MOVE_REQ,								// GUILD Warehouse ÀÇ ¾ÆÀÌÅÛ ÀÌµ¿
	UG_GUILD_BANK_MOVE_STACK_REQ,						// GUILD Warehouse ÀÇ ½ºÅÃ¾ÆÀÌÅÛ ÀÌµ¿
	UG_GUILD_BANK_END_REQ,								// GUILD Warehouse ¸¦ ´Ý´Â´Ù.
	UG_GUILD_BANK_ZENNY_REQ,							// GUILD Warehouse ¿¡ Àú±Ý È¤Àº ÀÎÃâÇÑ´Ù.
	UG_GUILD_CREATE_MARK_REQ,							// GuildMark Create
	UG_GUILD_CHANGE_MARK_REQ,							// GuildMark Change
	
	UG_CROSSFIRE_REQ,
	UG_BANK_LOAD_REQ,

	UG_GUILD_CHANGE_NAME_REQ,			

	UG_PARTY_SHARETARGET_REQ,							// [3/28/2008 SGpro]
	UG_RIDE_ON_BUS_REQ,
	UG_RIDE_OFF_BUS_REQ,

	UG_TRANSFORM_CANCEL_REQ,							// º¯½Å Ãë¼Ò ¿äÃ»
	UG_SHOP_ITEM_IDENTIFY_REQ,

	UG_DICE_ROLL_REQ,									// Action Skill ÁÖ»çÀ§ ±¼¸®±â

	UG_BUDOKAI_JOIN_INDIVIDUAL_REQ,						// °³ÀÎ µî·Ï
	UG_BUDOKAI_LEAVE_INDIVIDUAL_REQ,					// °³ÀÎ Ãë¼Ò

	UG_BUDOKAI_JOIN_TEAM_INFO_REQ,						//
	UG_BUDOKAI_JOIN_TEAM_REQ,							// ÆÀ µî·Ï
	UG_BUDOKAI_LEAVE_TEAM_REQ,							// ÆÀ Ãë¼Ò
	UG_BUDOKAI_LEAVE_TEAM_MEMBER_REQ,					// ÆÀ ¸â¹ö Ãë¼Ò

	UG_BUDOKAI_JOIN_INFO_REQ,							// Âü°¡ Á¤º¸ ¿äÃ»
	UG_BUDOKAI_JOIN_STATE_REQ,							// Âü°¡ »óÅÂ ¿äÃ»
	UG_BUDOKAI_MUDOSA_INFO_REQ,							// ¹«µµ»ç Á¤º¸ ¿äÃ»
	UG_BUDOKAI_MUDOSA_TELEPORT_REQ,						// ¹«µµ»ç·Î ÅÚ·¹Æ÷Æ® ¿äÃ»
	UG_BUDOKAI_PARTY_MAKER_REQ,							// ÆÄÆ¼ »ý¼º ¿äÃ», ÃµÇÏÁ¦ÀÏ¹«µµÈ¸ Å×½ºÆ®¿ë
	UG_BUDOKAI_SOCIAL_ACTION,
	UG_BUDOKAI_PRIZEWINNER_NAME_REQ,					// ¿ì½ÂÀÚ / ÁØ¿ì½ÂÀÚ : °³ÀÎÀü
//	UG_BUDOKAI_PRIZEWINNER_TEAM_NAME_REQ,				// ¿ì½ÂÀÚ / ÁØ¿ì½ÂÀÚ : ÆÀÀü

	UG_BUDOKAI_GM_BUDOKAI_SERVER_ENTER_REQ,				// ÃµÇÏÁ¦ÀÏ¹«µµÈ¸ ¼­¹ö·Î ÅÚ·¹Æ÷Æ® ¿äÃ»
	UG_BUDOKAI_GM_BUDOKAI_SERVER_LEAVE_REQ,				// ÃµÇÏÁ¦ÀÏ¹«µµÈ¸ ¼­¹ö¿¡¼­ ¿ø·¡ ¼­¹ö·Î ÀÌµ¿
	UG_BUDOKAI_GM_MATCH_PROGRESS_INFO_REQ,				// °æ±â ÁøÇà Á¤º¸ ¿äÃ»
	UG_BUDOKAI_GM_MATCH_ARENA_ENTER_REQ,				// °æ±âÀå µé¾î°¡±â
	UG_BUDOKAI_GM_MATCH_ARENA_LEAVE_REQ,				// °æ±âÀå ³ª°¡±â

	UG_SCOUTER_TURN_ON,									// ½ºÄ«¿ìÅÍ »óÅÂ ¿Â
	UG_SCOUTER_TURN_OFF,								// ½ºÄ«¿ìÅÍ »óÅÂ ¿ÀÇÁ
	UG_SCOUTER_ITEM_SELL_REQ,							// ½ºÄ«¿ìÅÍ¸¦ ÅëÇÑ ¾ÆÀÌÅÛ ÆÈ±â

	UG_SHOP_EVENTITEM_START_REQ,						// [7/15/2008 SGpro]
	UG_SHOP_EVENTITEM_BUY_REQ,							// [7/11/2008 SGpro]
	UG_SHOP_EVENTITEM_END_REQ,							// [7/15/2008 SGpro]

	UG_SHOP_GAMBLE_BUY_REQ,							// [7/11/2008 SGpro]
	
	UG_PARTY_DIFF_CHANGE_REQ,							// party dungeon diff change
	UG_PARTY_DUNGEON_INIT_REQ,							// party dungeon Init

	UG_SKILL_INIT_REQ,

	UG_PARTY_CHARM_DICE_ROLL_REQ,							// ÆÄÆ¼Âü µî·Ï ½Ã ÁÖ»çÀ§ ±¼¸®±â
	UG_PARTY_INVEN_LOCK_REQ,
	UG_PARTY_INVEN_RESET_REQ,
	UG_PARTY_INVEN_ACCEPTED_REQ,
	UG_PARTY_INVEN_GIVE_REQ,
	UG_PARTY_INVEN_DICE_START_REQ,
	UG_PARTY_INVEN_DICE_ROLL_REQ,					// ¾ÆÀÌÅÛ ºÐ¹è½Ã ÁÖ»çÀ§ ±¼¸®±â

	UG_TELEPORT_CONFIRM_REQ,
	
	UG_HOIPOIMIX_ITEM_CHECK_REQ,					// ¸¸µé¾ÆÀÌÅÛ ¼±ÅÃ (ÇÊ¿äÇÑ ¾ÆÀÌÅÛ ¼ö·® Ã¼Å©)
	UG_HOIPOIMIX_ITEM_MAKE_REQ,						// ¾ÆÀÌÅÛ ¸¸µé±â
	UG_HOIPOIMIX_ITEM_MAKE_EP_REQ,					// ¾ÆÀÌÅÛ ¸¸µé±â Áß EP ³Ö±â
	UG_HOIPOIMIX_ITEM_MACHINE_MAKE_REQ,				// ¸Ó½Å ¸¸µé±â
	UG_HOIPOIMIX_ITEM_MACHINE_DEL_REQ,				// ¸Ó½Å »èÁ¦
	UG_HOIPOIMIX_JOB_SET_REQ,						// ¹Í½º Àâ ¼¼ÆÃ
	UG_HOIPOIMIX_JOB_RESET_REQ,						// ¹Í½º Àâ ¸®¼Â

	UG_VEHICLE_DIRECT_PLAY_CANCEL_NFY,
	UG_VEHICLE_ENGINE_START_REQ,					// Å» °Í ½Ãµ¿ ÄÒ´Ù.
	UG_VEHICLE_ENGINE_STOP_REQ,						// Å» °Í ½Ãµ¿ ²ö´Ù.
	UG_VEHICLE_END_REQ,								// Å» °Í »ç¿ë ÁßÁö.
	UG_VEHICLE_STUNT_NFY,							// ½ºÆäÀÌ½º ¹Ù ´©¸§(Å»°Í)
	UG_VEHICLE_FUEL_REMOVE_NFY,

	// ½Â°´ °ü·Ã ÀÛ¾÷Àº ´ÙÀ½ ¸¶ÀÏ½ºÅæ~ 2009.1.5 ½Â¿ø.
	UG_VEHICLE_PASSENGER_GETON_REQ,
	UG_VEHICLE_ASK_RES,
	UG_VEHICLE_PASSENGER_GETOFF_REQ,

	UG_DOJO_CREATE_REQ,
	UG_DOJO_DELETE_REQ,
	UG_DOJO_FUNCTION_ADD_REQ,
	UG_DOJO_BANK_HISTORY_REQ,
	UG_DOGI_CREATE_REQ,								// µµº¹»ý¼º
	UG_DOGI_CHANGE_REQ,								// µµº¹º¯°æ
	UG_GUILD_DOGI_CREATE_REQ,						// À¯ÆÄµµº¹»ý¼º
	UG_GUILD_DOGI_CHANGE_REQ,						// À¯ÆÄµµº¹º¯°æ
	UG_DOJO_SCRAMBLE_REQ,							// µµÀåÀïÅ»Àü ½ÅÃ»
	UG_DOJO_SCRAMBLE_RESPONSE_REQ,					// µµÀåÀïÅ»Àü ½ÅÃ»¿¡ ´ëÇÑ ÀÀ´ä

	UG_SHOP_NETPYITEM_START_REQ,
	UG_SHOP_NETPYITEM_BUY_REQ,
	UG_SHOP_NETPYITEM_END_REQ,

	UG_WORLD_MAP_STATUS,
	UG_DOJO_NPC_INFO_REQ,							// µµÀåNPC¿¡°Ô Á¤º¸¿äÃ»

	UG_GAMEGUARD_AUTH_ANSWER_RES,
	UG_GAMEGUARD_HACK_INFO_NFY,

	UG_SKILL_CASTING_CANCELED_NFY,					// ½ºÅ³ Ä³½ºÆÃ Ãë¼Ò ¾Ë¸²

	UG_PET_ATTACK_TARGET_NFY,						// ÆêÀÇ ÀüÅõ Âü¿©

	UG_DURATION_ITEM_BUY_REQ,						// »óÁ¡¿¡¼­ ±¸¸Å½Ã
	UG_DURATION_RENEW_REQ,							// ±â°£Á¦ ¾ÆÀÌÅÛ ±â°£¸¸ °»½Å¿äÃ»½Ã

	UG_CASHITEM_START_REQ,
	UG_CASHITEM_END_REQ,
	UG_CASHITEM_MOVE_REQ,
	UG_CASHITEM_USE_REQ,
	UG_CASHITEM_HLSHOP_START_REQ,					// Ä³½Ã »óÁ¡ ¿­¶§
	UG_CASHITEM_HLSHOP_END_REQ,
	UG_CASHITEM_BUY_REQ,							// Ä³½¬ ¾ÆÀÌÅÛ ±¸¸Å½Ã

	UG_BUDOKAI_PRIZEWINNER_TEAM_NAME_REQ,				// ¿ì½ÂÀÚ / ÁØ¿ì½ÂÀÚ : ÆÀÀü
	UG_GMT_UPDATE_REQ,

	UG_CHAR_LOC_AFTER_KNOCKDOWN_NFY,
	UG_CHAR_LOC_AFTER_SLIDING_NFY,
	UG_CHAR_LOC_AFTER_PUSH_NFY,*/

	UG_OPCODE_DUMMY,
	UG_OPCODE_END = UG_OPCODE_DUMMY - 1
};


//------------------------------------------------------------------
//
//------------------------------------------------------------------
const char * NtlGetPacketName_UG(WORD wOpCode);
//------------------------------------------------------------------

#pragma pack(1)
//------------------------------------------------------------------

BEGIN_PROTOCOL(UG_GAME_ENTER_REQ)
	ACCOUNTID			accountId;
	CHARACTERID			charId;
	BYTE				abyAuthKey[NTL_MAX_SIZE_AUTH_KEY];
	bool				bTutorialMode;	// Æ©Åä¸®¾ó ¿äÃ» °ª
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_GAME_LEAVE_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_AUTH_KEY_FOR_COMMUNITY_SERVER_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_ENTER_WORLD)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SERVER_COMMAND)
	WORD				wMessageLengthInUnicode;
	WCHAR				awchCommand[NTL_MAX_LENGTH_OF_CHAT_MESSAGE_UNICODE + 1];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_READY_FOR_COMMUNITY_SERVER_NFY)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_READY_TO_SPAWN)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_READY)
	BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_MOVE)
//- yoshiki : Do we use dwTimeStamp or not?
uint8_t avatar_type;
sVECTOR3            vCurLoc;
sVECTOR3            vCurDir;
uint8_t move_type;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_AIR_MOVE)
//- yoshiki : Do we use dwTimeStamp or not?
BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
sVECTOR3			vCurLoc;
sVECTOR3			vCurDir;
BYTE				byMoveDirection;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_AIR_MOVE_SYNC)
//- yoshiki : Do we use dwTimeStamp or not?
BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
sVECTOR3			vCurLoc;
sVECTOR3			vCurDir;
BYTE				byMoveDirection;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_DEST_MOVE)
	//- yoshiki : Do we use dwTimeStamp or not?
	DWORD				dwTimeStamp;
	BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
	sVECTOR3			vCurLoc;
	sVECTOR3			vDestLoc;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_CHANGE_HEADING)
	//- yoshiki : Do we use dwTimeStamp or not?
	DWORD				dwTimeStamp;
	sVECTOR3			vCurrentPosition;
	sVECTOR3			vCurrentHeading;
END_PROTOCOL()
//------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_CHAR_MOVE_SYNC)
////	//- yoshiki : Do we use dwTimeStamp or not?
//	DWORD				dwTimeStamp;
//	BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
//	sVECTOR3			vCurLoc;
//	sVECTOR3			vCurDir;
//END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_CHANGE_DIRECTION_ON_FLOATING)
	//- yoshiki : Do we use dwTimeStamp or not?
	DWORD				dwTimeStamp;
	sVECTOR3			vCurDir;
	BYTE				byMoveDirection;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_FOLLOW_MOVE)
	//- yoshiki : Do we use dwTimeStamp or not?
	DWORD				dwTimeStamp;
	BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
	HOBJECT				hTarget;
	float				fDistance;
	BYTE				byMovementReason;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_DASH_KEYBOARD)
	//- yoshiki : Do we use dwTimeStamp or not?
	DWORD				dwTimeStamp;
	sVECTOR3			vCurLoc;
	sVECTOR2			vCurDir;
	BYTE				byMoveDirection;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_AIR_DASH)
	//- yoshiki : Do we use dwTimeStamp or not?
	DWORD				dwTimeStamp;
	sVECTOR3			vCurLoc;
	sVECTOR2			vCurDir;
	BYTE				byMoveDirection;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_DASH_MOUSE)
	//- yoshiki : Do we use dwTimeStamp or not?
	DWORD				dwTimeStamp;
	sVECTOR3			vDestLoc;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_JUMP)
	sVECTOR3			vCurrentHeading;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_AIR_JUMP)
sVECTOR3			vCurrentHeading;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_JUMP_END)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_FALLING)
	bool				bIsFalling;
	sVECTOR3			vCurLoc;
	sVECTOR2			vCurDir;
	BYTE				byMoveDirection;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_TOGG_SITDOWN)
	bool				bSitDown;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_TOGG_FIGHTING)
	BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
	bool				bFightMode;
END_PROTOCOL()
//------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_CHAR_TOGG_RUNNING)
//	bool				bRunMode;
//END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_TARGET_SELECT)
	BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
	HOBJECT				hTarget;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_TARGET_INFO)
	HOBJECT				hTarget;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_TARGET_FACING)
	HOBJECT				hTarget;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_ATTACK_BEGIN)
	BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
	BYTE				byType;	// Çü¼® : ÀÓ½Ã
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_ATTACK_END)
	BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
	BYTE				byType;	// Çü¼® : ÀÓ½Ã
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_CHARGE)
	bool				bCharge; // ON/OFF
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_BLOCK_MODE)
	bool				bIsOn;		// On/Off
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_TELEPORT_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_BIND_REQ)
	TBLIDX				bindObjectTblidx;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_REVIVAL_REQ)
	BYTE				byRevivalRequestType;		// eDBO_REVIVAL_REQUEST_TYPE
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_SERVER_CHANGE_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_CHANNEL_CHANGE_REQ)
	SERVERCHANNELID		destServerChannelId;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_EXIT_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_GAME_EXIT_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_CHAR_AWAY_REQ )
	bool	bIsAway;		// AwayOn 1: Awayoff: 0
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_CHAR_KEY_UPDATE_REQ )
	BYTE	byCount;		// º¯°æ°¹¼ö
	sSHORTCUT_UPDATE_DATA asData[NTL_SHORTCUT_MAX_COUNT];	 
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_CHAR_DIRECT_PLAY_ACK ) 		
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_CHAR_KNOCKDOWN_RELEASE_NFY ) 		
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_SKILL_REQ)
	BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
	BYTE				bySlotIndex;
	BYTE				byRpBonusType;		// eDBO_RP_BONUS_TYPE
	HOBJECT				hTarget;
	// If skill effect should be applied on a target character, too, the target character must be included in both byApplyTargetCount and ahApplyTarget.
	BYTE				byApplyTargetCount;
	HOBJECT				ahApplyTarget[NTL_MAX_NUMBER_OF_SKILL_TARGET];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SKILL_TARGET_LIST)
	BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
	// If skill effect should be applied on a target character, too, the target character must be included in both byApplyTargetCount and ahApplyTarget.
	BYTE				byApplyTargetCount;
	HOBJECT				ahApplyTarget[NTL_MAX_NUMBER_OF_SKILL_TARGET];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SKILL_LEARN_REQ)
	TBLIDX				skillTblidx;
END_PROTOCOL()
BEGIN_PROTOCOL(UG_SKILL_LEARN_BY_ITEM_REQ)
TBLIDX				skillTblidx;
BYTE test;
WORD test1;

END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SKILL_UPGRADE_REQ)
	BYTE				bySlotIndex;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SKILL_RP_BONUS_SETTING_REQ)
	BYTE				bySlotIndex;
	BYTE				byRpBonusType;		// eDBO_RP_BONUS_TYPE
	bool				bIsRpBonusAuto;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_HTB_START_REQ)
	HOBJECT				hTarget;
	BYTE				bySkillSlot;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_HTB_LEARN_REQ)
	HOBJECT				hTrainer; // Æ®·¹ÀÌ³Ê ÇÚµé
	TBLIDX				skillId; // HTB set Å×ÀÌºí ¹øÈ£
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_HTB_FORWARD_REQ)
	sVECTOR3			vCurLoc; // ÁøÇà À§Ä¡
	sVECTOR2			vCurDir; // ÁøÇà ¹æÇâ
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_HTB_RP_BALL_USE_REQ)
	BYTE				byRpBallCount;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUFF_DROP_REQ)
	BYTE				bySourceType;		// eDBO_OBJECT_SOURCE
	TBLIDX				tblidx;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_ITEM_MOVE_REQ)
	BYTE				bySrcPlace;
	BYTE				bySrcPos;
	BYTE				byDestPlace;
	BYTE				byDestPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_ITEM_MOVE_STACK_REQ)
	BYTE				bySrcPlace;
	BYTE				bySrcPos;
	BYTE				byDestPlace;
	BYTE				byDestPos;
	BYTE				byStackCount; 
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_ITEM_DELETE_REQ)
	BYTE				bySrcPlace;
	BYTE				bySrcPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_ITEM_EQUIP_REPAIR_REQ)
	HOBJECT				handle;			// NpcHandle
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_ITEM_PICK_REQ)
	BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
	HOBJECT				handle;			// ItemHandle
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_ITEM_REPAIR_REQ)
	HOBJECT				handle;			// NpcHandle
	BYTE				byPlace;
	BYTE				byPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_ITEM_USE_REQ)
	HOBJECT				hTarget;
	BYTE				byPlace;
	BYTE				byPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_ITEM_UPGRADE_REQ)
	HOBJECT				handle;				// NpcHandle
	BYTE				byItemPlace;		// ¾÷±×·¹ÀÌµå ÇÒ ¾ÆÀÌÅÛÀÇ ÄÁÅ×ÀÌ³Ê À§Ä¡
	BYTE				byItemPos;			// ¾÷±×·¹ÀÌµå ÇÒ ¾ÆÀÌÅÛÀÇ À§Ä¡
	BYTE				byStonPlace;		// È£ÀÌÆ÷ÀÌ½ºÅæ ÄÁÅ×ÀÌ³Ê À§Ä¡
	BYTE				byStonPos;			// È£ÀÌÆ÷ÀÌ½ºÅæ À§Ä¡
	BYTE				byPosNum;			// ¾÷±×·¹ÀÌµå Ã¢ÀÇ Á¶ÇÕÀ§Ä¡: (¾ÆÀÌÅÛÀÇ ½½·Ô¹øÈ£ * 10 + È£ÀÌÆ÷ÀÌ½ºÅæ½½·Ô¹øÈ£) 
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_ITEM_IDENTIFY_REQ)
	BYTE				byPlace;			// ¾ÆÀÌÅÛ È®ÀÎ½ÃÀÇ È®ÀÎ ÇÒ ¾ÆÀÌÅÛÀÇ ÄÁÅ×ÀÌ³Ê À§Ä¡
	BYTE				byPos;				// ¾ÆÀÌÅÛÀÇ Æ÷Áö¼Ç.
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_ZENNY_PICK_REQ)
	BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
	HOBJECT				handle;			// ItemHandle
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SHOP_START_REQ)
	HOBJECT				handle;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SHOP_BUY_REQ)
	HOBJECT				handle;
	BYTE				byBuyCount;
	sSHOP_BUY_CART		sBuyData[NTL_MAX_BUY_SHOPPING_CART]; 
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SHOP_SELL_REQ)
	HOBJECT				handle;
	BYTE				bySellCount;
	sSHOP_SELL_CART		sSellData[NTL_MAX_SELL_SHOPPING_CART]; // 3BYTE * 8 = 24
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SHOP_END_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SHOP_SKILL_BUY_REQ)
	HOBJECT				hNpchandle;
	BYTE				byMerchantTab;
	BYTE				byPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PARTY_CREATE_REQ)
	WCHAR				wszPartyName[NTL_MAX_SIZE_PARTY_NAME_IN_UNICODE + 1];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PARTY_DISBAND_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PARTY_INVITE_REQ)
	HOBJECT				hTarget;
	// If the player belong to a party, the part of 'wszPartyName' doesn't need to be sent when inviting another player.
	// ÀÌ¹Ì ÆÄÆ¼¿¡ °¡ÀÔÇÑ »óÅÂ¿¡¼­ ´Ù¸¥ ÇÃ·¹ÀÌ¾î¸¦ ÃÊ´ëÇÒ ¶§¿¡´Â wszPartyName ºÎºÐÀ» Àü¼ÛÇÏÁö ¾Ê¾Æµµ µÈ´Ù.
	// by YOSHIKI(2009-02-13)
	WCHAR				wszPartyName[NTL_MAX_SIZE_PARTY_NAME_IN_UNICODE + 1];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PARTY_INVITE_CHARID_REQ)
	CHARACTERID			targetCharId;
	// If the player belong to a party, the part of 'wszPartyName' doesn't need to be sent when inviting another player.
	// ÀÌ¹Ì ÆÄÆ¼¿¡ °¡ÀÔÇÑ »óÅÂ¿¡¼­ ´Ù¸¥ ÇÃ·¹ÀÌ¾î¸¦ ÃÊ´ëÇÒ ¶§¿¡´Â wszPartyName ºÎºÐÀ» Àü¼ÛÇÏÁö ¾Ê¾Æµµ µÈ´Ù.
	// by YOSHIKI(2009-02-13)
	WCHAR				wszPartyName[NTL_MAX_SIZE_PARTY_NAME_IN_UNICODE + 1];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PARTY_INVITE_CHAR_NAME_REQ)
	WCHAR				wszTargetName[NTL_MAX_SIZE_CHAR_NAME_UNICODE + 1];
	// If the player belong to a party, the part of 'wszPartyName' doesn't need to be sent when inviting another player.
	// ÀÌ¹Ì ÆÄÆ¼¿¡ °¡ÀÔÇÑ »óÅÂ¿¡¼­ ´Ù¸¥ ÇÃ·¹ÀÌ¾î¸¦ ÃÊ´ëÇÒ ¶§¿¡´Â wszPartyName ºÎºÐÀ» Àü¼ÛÇÏÁö ¾Ê¾Æµµ µÈ´Ù.
	// by YOSHIKI(2009-02-13)
	WCHAR				wszPartyName[NTL_MAX_SIZE_PARTY_NAME_IN_UNICODE + 1];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PARTY_RESPONSE_INVITATION)
	BYTE				byResponse;		 //eNTL_INVITATION_RESPONSE
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PARTY_LEAVE_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PARTY_KICK_OUT_REQ)
	HOBJECT				hTargetMember;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PARTY_CHANGE_LEADER_REQ)
	HOBJECT				hTargetMember;
END_PROTOCOL()
//------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_OPEN_CHARM_SLOT_REQ)
//	BYTE				byCharmInventoryIndex;
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_CLOSE_CHARM_SLOT_REQ)
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_REGISTER_VICTIM_ITEM_REQ)
//	BYTE				bySrcPlace;
//	BYTE				bySrcPos;
//	BYTE				byDestSlotIndex;
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_UNREGISTER_VICTIM_ITEM_REQ)
//	BYTE				byDestSlotIndex;
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_REGISTER_VICTIM_ZENNY_REQ)
//	DWORD				dwZenny;
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_UNREGISTER_VICTIM_ZENNY_REQ)
//	DWORD				dwZenny;
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_DECLARE_ZENNY_REQ)
//	DWORD				dwZennyToDeclare;
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_ACTIVATE_CHARM_REQ)
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_DEACTIVATE_CHARM_REQ)
//	BYTE				byCharmInventoryIndex;
//END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PARTY_CHANGE_ZENNY_LOOTING_METHOD_REQ)
	BYTE				byLootingMethod;		// eNTL_PARTY_ZENNY_LOOTING
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PARTY_CHANGE_ITEM_LOOTING_METHOD_REQ)
	BYTE				byLootingMethod;		// eNTL_PARTY_ITEM_LOOTING
END_PROTOCOL()
//------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_INVEST_ZENNY_REQ)
//	BYTE			byPos;
//	// 'dwZennyToInvest' doesn't mean relative value to the current zenny but final invested zenny.
//	// ÇöÀç ÅõÀÚµÈ Æ÷ÀÎÆ®¿ÍÀÇ »ó´ëÀûÀÎ °ªÀÌ ¾Æ´Ï¶ó, ¾ÆÀÌÅÛ¿¡ ÃÖÁ¾ÀûÀ¸·Î ÅõÀÚµÉ Á¦´ÏÀÌ´Ù.
//	// by YOSHIKI(2006-11-01)
//	DWORD			dwZennyToInvest;
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_INVEN_ITEM_RANK_SET_REQ)
//	BYTE				byItemRank; // eITEM_RANK 
//END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BANK_START_REQ)
	HOBJECT				handle;			// NpcHandle
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BANK_MOVE_REQ)
	HOBJECT				handle;
	BYTE				bySrcPlace;
	BYTE				bySrcPos;
	BYTE				byDestPlace;
	BYTE				byDestPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BANK_MOVE_STACK_REQ)
	HOBJECT				handle;
	BYTE				bySrcPlace;
	BYTE				bySrcPos;
	BYTE				byDestPlace;
	BYTE				byDestPos;
	BYTE				byStackCount;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BANK_END_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BANK_ZENNY_REQ)
	HOBJECT				handle;			// NpcHandle
	DWORD				dwZenny;		// »©°Å³ª ³ÖÀ» ¾×¼ö
	bool				bIsSave;		// 1 Àº ³ÖÀ» °æ¿ì 0 Àº »©´Â°æ¿ì
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BANK_BUY_REQ)
	HOBJECT				hNpchandle;
	BYTE				byMerchantTab;
	BYTE				byPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BANK_ITEM_DELETE_REQ)
	BYTE				byPlace;
	BYTE				byPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SCOUTER_INDICATOR_REQ)
	HOBJECT				hTarget;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SCOUTER_PREDICT_REQ)
	BYTE				byPusNum;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SCOUTER_EQUIP_CHECK_REQ)  
	HOBJECT				hTarget;			// È®ÀÎ ÇÏ°íÀÚ ÇÏ´Â »ó´ë Å¸°Ù ÇÚµé
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SCOUTER_CHIP_REMOVE_ALL_REQ)  
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SOCIAL_ACTION)
	TBLIDX				socialActionId;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TS_CONFIRM_STEP_REQ)  
	BYTE				byTsType; // Æ®¸®°Å Å¸ÀÔ
	NTL_TS_T_ID			tId;
	NTL_TS_TC_ID		tcCurId;
	NTL_TS_TC_ID		tcNextId;
	DWORD				dwParam;
	BYTE				byEventType;
	DWORD				dwEventData;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TS_UPDATE_STATE)
	BYTE				byTsType;	// Æ®¸®°Å Å¸ÀÔ
	NTL_TS_T_ID			tId;
	BYTE				byType;
	WORD				wTSState;
	DWORD				dwParam;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TS_EXCUTE_TRIGGER_OBJECT)
	HOBJECT				hSource;
	HOBJECT				hTarget;
	BYTE				byEvtGenType;
	unsigned int		uiParam;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_QUEST_ITEM_MOVE_REQ)  
	BYTE				bySrcPos;
	BYTE				byDestPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_QUEST_ITEM_DELETE_REQ)  
	BYTE				byDeletePos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_QUEST_GIVEUP_REQ)  
	NTL_TS_T_ID			tId;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_QUEST_SHARE)  
	NTL_TS_T_ID			tId;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_QUEST_OBJECT_VISIT_REQ)  
	QUESTID				qId;
	WORLDID				worldId;
	BYTE				byObjType;			// eOBJTYPE
	TBLIDX				objectTblidx;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_FREEBATTLE_CHALLENGE_REQ)  
	HOBJECT				hTarget;	// ´ëÀü½ÅÃ» »ó´ëÄ³¸¯ÅÍÀÇ ÇÚµé
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_FREEBATTLE_ACCEPT_RES)  
	BYTE				byAccept; // 1: ¼ö¶ô 0: °ÅÀý 2: ¼ö¶ôÇÒ»óÅÂ°¡ ¾Æ´Ô ´ëÀü ¼ö¶ô ¿©ºÎ
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_QUICK_SLOT_UPDATE_REQ)  
	TBLIDX				tblidx;
	BYTE				bySlotID;
	BYTE				byType;
	BYTE				byPlace;
	BYTE				byPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_QUICK_SLOT_DEL_REQ)  
	BYTE				bySlotID;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PET_DISMISS_PET_REQ)
	BYTE				byAvatarType;		// eDBO_AVATAR_TYPE
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TRADE_START_REQ)			// ÇØ´ç À¯Àú¿ÍÀÇ ±³È¯ ¿äÃ»
	HOBJECT				hTarget;			// Å¸°Ù ÇÚµé
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TRADE_OK_RES)				// ÇØ´ç À¯Á®ÀÇ ÀÀ´ä
	HOBJECT				handle;				// ¿äÃ»ÀÚÀÇ ÇÚµé
	BYTE				byOK;				// ½Â¶ô À¯¹« 1 : ½Â¶ô 0 : °ÅºÎ 3: ¹Ù»Û»óÈ²
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TRADE_ADD_REQ)			// ±³È¯Ã¢¿¡ ¾ÆÀÌÅÛ µî·Ï
	HOBJECT				hTarget;			// Å¸°Ù ÇÚµé
	HOBJECT				hItem;				// ¾ÆÀÌÅÛ ÇÚµé
	BYTE				byCount;			// °ãÃÄÀÖ´Â ¾ÆÀÌÅÛÁß ¸î°³ÀÎ°¡? Default 1°³
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TRADE_DEL_REQ)			// ±³È¯Ã¢¿¡ ¾ÆÀÌÅÛ »èÁ¦
	HOBJECT				hTarget;			// Å¸°Ù ÇÚµé
	HOBJECT				hItem;				// ¾ÆÀÌÅÛ ÇÚµé
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TRADE_MODIFY_REQ)			// ±³È¯Ã¢¿¡ ¾ÆÀÌÅÛ µî·Ï
	HOBJECT				hTarget;			// Å¸°Ù ÇÚµé
	HOBJECT				hItem;				// ¾ÆÀÌÅÛ ÇÚµé
	BYTE				byCount;			// °ãÃÄÀÖ´Â ¾ÆÀÌÅÛÁß ¸î°³ÀÎ°¡? Default 1°³
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TRADE_ZENNY_UPDATE_REQ)	// ±³È¯Ã¢¿¡ Á¦´Ï µî·Ï ¹× º¯°æ
	HOBJECT				hTarget;			// TargetHandle
	DWORD				dwZenny;			// ¿Å±æ ¸¸Å­ÀÇ Á¦´Ï (ÀÚ±â°¡ °®°í ÀÖ´Â Á¦´Ï ÇÑµµ ³»)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TRADE_END_REQ)			// ±³È¯ ¿Ï·á ¹öÆ° Å¬¸¯
	HOBJECT				hTarget;			// Å¸°Ù ÇÚµé
	DWORD				dwPacketCount;		// »ó´ë¹æÀÇ º¯°æ Ä«¿îÆ® ( »ó´ë¹æÀÌ ¹°°ÇÀ» ¿Ã·Á³õ°Å³ª »èÁ¦ È¤Àº º¯°æ µî ÆÐÅ¶¹ÞÀº °³¼ö )
	bool				bIsSet;				// false : È®ÀÎÇ®±â true: È®ÀÎÀá±Ý
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TRADE_CANCEL_REQ)			// ±³È¯ Ãë¼Ò È¤Àº Ä³¸¯Á¾·á °Å¸® ÀÌÅ»½Ã »ç¿ë
	HOBJECT				hTarget;			// Å¸°Ù ÇÚµé
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TRADE_DENY_REQ)			// ±³È¯ Ãë¼Ò È¤Àº Ä³¸¯Á¾·á °Å¸® ÀÌÅ»½Ã »ç¿ë
	bool				bIsDeny;			// TRUE: °ÅÀý ÇÏµµ·Ï FALSE: ÇØÁ¦ ¿ø»óÅÂ
END_PROTOCOL()
//------------------------------------------------------------------	
BEGIN_PROTOCOL(UG_GUILD_CREATE_REQ)
	HOBJECT				hGuildManagerNpc;
	WCHAR				wszGuildName[NTL_MAX_SIZE_GUILD_NAME_IN_UNICODE + 1];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_GUILD_INVITE_REQ)
	HOBJECT				hTarget;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TUTORIAL_HINT_UPDATE_REQ)
	DWORD				dwTutorialHint;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TUTORIAL_WAIT_CANCEL_REQ)

END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TUTORIAL_PLAY_QUIT_REQ)

END_PROTOCOL()
//------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_TIMEQUEST_ROOM_LIST_REQ)
//	HOBJECT				hTimeQuestNpc;		// ÇØ´ç Å¸ÀÓ¸Ó½ÅÄù½ºÆ® NPCÀÇ ÇÚµé 
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_TIMEQUEST_ROOM_INFO_REQ)
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_TIMEQUEST_ROOM_JOIN_REQ)
//	HOBJECT				hTimeQuestNpc;		// ÇØ´ç Å¸ÀÓ¸Ó½ÅÄù½ºÆ® NPCÀÇ ÇÚµé
//	BYTE				byTimeQuestMode;	// °³ÀÎ or Party ( eTIMEQUEST_MODE )
//	bool				bIsRetry;			// Àç½ÅÃ»ÀÎÁö ¾Æ´ÑÁö¸¦ ³ªÅ¸³¿
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_TIMEQUEST_ROOM_LEAVE_REQ)
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_TIMEQUEST_ROOM_TELEPORT_REQ)
//END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_RANKBATTLE_INFO_REQ)
	BYTE					byBattleMode;		// eRANKBATTLE_MODE
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_RANKBATTLE_JOIN_REQ)
	HOBJECT					hBoardObject;
	TBLIDX					rankBattleTblidx;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_RANKBATTLE_LEAVE_REQ)
	TBLIDX					rankBattleTblidx;
END_PROTOCOL()
//------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_RANKBATTLE_ROOM_LIST_REQ)
//	HOBJECT				hRankBattleObject;	// °Ô½ÃÆÇ ObjectÀÇ ÇÚµé
//	WORD				wPage;				// °Ô½ÃÆÇÀÇ Ãâ·Â Page
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_RANKBATTLE_ROOM_INFO_REQ)
//	HOBJECT				hRankBattleObject;	// °Ô½ÃÆÇ ObjectÀÇ ÇÚµé
//	ROOMID				roomId;
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_RANKBATTLE_ROOM_CREATE_REQ)
//	HOBJECT				hRankBattleObject;	// °Ô½ÃÆÇ ObjectÀÇ ÇÚµé
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_RANKBATTLE_ROOM_LEAVE_REQ)
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_RANKBATTLE_ROOM_CHALLENGE_REQ)
//	HOBJECT				hRankBattleObject;	// °Ô½ÃÆÇ ObjectÀÇ ÇÚµé
//	ROOMID				roomId;				// ¹æ ¹øÈ£
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_RANKBATTLE_ROOM_MATCH_CANCEL_REQ)
//END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_DRAGONBALL_CHECK_REQ)
	HOBJECT				hObject;						  // TS Object Á¦´Ü	 
	sITEM_POSITION_DATA sData[NTL_ITEM_MAX_DRAGONBALL];   // ÀÎº¥»óÀÇ µå·¡°ïº¼¾ÆÀÌÅÛ
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_DRAGONBALL_REWARD_REQ)
	HOBJECT				hObject;						  // TS Object Á¦´Ü	
	TBLIDX				rewardTblidx;					  // ¼±ÅÃ º¸»ó Å×ÀÌºí ÀÎµ¦½º
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_CREATE_REQ )
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_EXIT_REQ )
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_OPEN_REQ )
	WCHAR					wcPrivateShopName[NTL_MAX_PRIVATESHOP_NAME_IN_UNICODE + 1];	
	bool					bIsOwnerEmpty; //trueÀÌ¸é ÁÖÀÎÀÌ ÀÚ¸®¸¦ ºñ¿î°Í
	BYTE					byNoticeSize;
	WCHAR					wcNotice[NTL_MAX_PRIVATESHOP_NOTICE_IN_UNICODE + 1];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_CLOSE_REQ )
END_PROTOCOL()
//------------------------------------------------------------------
//¾ÆÁ÷ ¹ÌÁ¤ (»ç¿ëÇÏÁö ¸¶¼¼¿ä - »èÁ¦ °¡´É¼º ÀÖÀ½)
BEGIN_PROTOCOL( UG_PRIVATESHOP_ITEM_GET_REQ )
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_ITEM_INSERT_REQ )
	BYTE							byPrivateShopInventorySlotPos;
	BYTE							byInventoryPlace;
	BYTE							byInventoryPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_ITEM_UPDATE_REQ )
	BYTE							byPrivateShopInventorySlotPos;
	DWORD							dwZenny;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_ITEM_DELETE_REQ )
	BYTE							byPrivateShopInventorySlotPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_ITEM_BUYING_REQ )
	HOBJECT						hOwner;
	BYTE						byPrivateShopInventorySlotPos[NTL_MAX_BUY_SHOPPING_CART];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_ENTER_REQ )
	HOBJECT						hOwner;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_LEAVE_REQ )
	HOBJECT						hOwner;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_ITEM_SELECT_REQ )
	HOBJECT						hOwner;
	BYTE						byPrivateShopInventorySlotPos;
	BYTE						byItemState;//eITEMSTATE °ªÀ» °®´Â´Ù
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_MAIL_START_REQ )
	HOBJECT	hObject;	// ¸ÞÀÏ¼Û¼ö½ÅÅ¾ ¿ÀºêÁ§Æ® (½ºÄ«¿ìÆ®ÆÄÃ÷ Âø¿ë½Ã INVALID_OBJECT )
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_MAIL_SEND_REQ )
	HOBJECT	hObject;	// ¸ÞÀÏ¼Û¼ö½ÅÅ¾ ¿ÀºêÁ§Æ® (½ºÄ«¿ìÆ®ÆÄÃ÷ Âø¿ë½Ã INVALID_OBJECT )
	BYTE byMailType;	// eMAIL_TYPE	
	sITEM_POSITION_DATA sItemData;	// ¾ÆÀÌÅÛÆ÷Áö¼Ç
	DWORD dwZenny;		// Req or Give Zenny
	BYTE byDay;		// À¯È¿¸¸·á³¯Â¥ ÃÖ´ë 10ÀÏ
	WCHAR wszTargetName[NTL_MAX_SIZE_CHAR_NAME_UNICODE + 1];	// ¹ÞÀ» Ä³¸¯ ÀÌ¸§
	BYTE  byTextSize;	// °¡º¯±æÀÌ ÆÐÅ¶¿ë ÅØ½ºÆ® »çÀÌÁî
	WCHAR wszText[NTL_MAX_LENGTH_OF_MAIL_MESSAGE + 1];	// ¸ÞÀÏ³»¿ë
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_MAIL_READ_REQ )
	HOBJECT	hObject;	// ¸ÞÀÏ¼Û¼ö½ÅÅ¾ ¿ÀºêÁ§Æ® (½ºÄ«¿ìÆ®ÆÄÃ÷ Âø¿ë½Ã INVALID_OBJECT )
	MAILID	mailID;	// ¸ÞÀÏ ¾ÆÀÌµð
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_MAIL_DEL_REQ )
	HOBJECT	hObject;	// ¸ÞÀÏ¼Û¼ö½ÅÅ¾ ¿ÀºêÁ§Æ® (½ºÄ«¿ìÆ®ÆÄÃ÷ Âø¿ë½Ã INVALID_OBJECT )
	MAILID	mailID;	// ¸ÞÀÏ ¾ÆÀÌµð
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_MAIL_RETURN_REQ )
	HOBJECT	hObject;	// ¸ÞÀÏ¼Û¼ö½ÅÅ¾ ¿ÀºêÁ§Æ® (½ºÄ«¿ìÆ®ÆÄÃ÷ Âø¿ë½Ã INVALID_OBJECT )
	MAILID	mailID;	// ¸ÞÀÏ ¾ÆÀÌµð	
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_MAIL_RELOAD_REQ )
	HOBJECT	hObject;	// ¸ÞÀÏ¼Û¼ö½ÅÅ¾ ¿ÀºêÁ§Æ® (½ºÄ«¿ìÆ®ÆÄÃ÷ Âø¿ë½Ã INVALID_OBJECT )
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_MAIL_LOAD_REQ )
	HOBJECT	hObject;	// ¸ÞÀÏ¼Û¼ö½ÅÅ¾ ¿ÀºêÁ§Æ® (½ºÄ«¿ìÆ®ÆÄÃ÷ Âø¿ë½Ã INVALID_OBJECT )
	BYTE	byCount;
	MAILID	aMailID[NTL_MAX_MAIL_SLOT_COUNT];	// ¸ÞÀÏ ¾ÆÀÌµð
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_MAIL_ITEM_RECEIVE_REQ )
	HOBJECT	hObject;	// ¸ÞÀÏ¼Û¼ö½ÅÅ¾ ¿ÀºêÁ§Æ® (½ºÄ«¿ìÆ®ÆÄÃ÷ Âø¿ë½Ã INVALID_OBJECT )
	MAILID	mailID;	// ¸ÞÀÏ ¾ÆÀÌµð
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_MAIL_LOCK_REQ )
	HOBJECT	hObject;	// ¸ÞÀÏ¼Û¼ö½ÅÅ¾ ¿ÀºêÁ§Æ® (½ºÄ«¿ìÆ®ÆÄÃ÷ Âø¿ë½Ã INVALID_OBJECT )
	MAILID	mailID;	// ¸ÞÀÏ ¾ÆÀÌµð
	bool	bIsLock;		// Lock 1: Unlock: 0
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_MAIL_MULTI_DEL_REQ )
	HOBJECT	hObject;								// ¸ÞÀÏ¼Û¼ö½ÅÅ¾ ¿ÀºêÁ§Æ® (½ºÄ«¿ìÆ®ÆÄÃ÷ Âø¿ë½Ã INVALID_OBJECT )
	BYTE	byCount;
	MAILID	aMailID[NTL_MAX_COUNT_OF_MULTI_DEL];		// ¸ÞÀÏ ¾ÆÀÌµð
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_BUSINESS_REQUEST_REQ )
	HOBJECT						hOwner;
	BYTE						byPrivateShopInventorySlotPos;
	TBLIDX						itemNo;//Å¬¶óÀÌ¾ðÆ®¿Í ¼­¹ö¿¡ ¾ÆÀÌÅÛÀÌ µ¿ÀÏÇÑÁö °Ë»çÇÏ±â À§ÇÔ
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_BUSINESS_CONSENT_RES )
	bool						bResult;
	TBLIDX						itemNo;//Å¬¶óÀÌ¾ðÆ®¿Í ¼­¹ö¿¡ ¾ÆÀÌÅÛÀÌ µ¿ÀÏÇÑÁö °Ë»çÇÏ±â À§ÇÔ
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_BUSINESS_WAITTIMEOUT_NFY )
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_BUSINESS_PRICEFLUCTUATIONS_REQ )
	DWORD						dwPriceFluctuationsZenny;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_BUSINESS_IMMEDIATE_ITEM_BUYING_REQ )
	HOBJECT						hOwner;
	unsigned __int16			nPriceFluctuationsCounter;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PRIVATESHOP_BUSINESS_CANCEL_REQ )
	HOBJECT						hOwner;
END_PROTOCOL()
//------------------------------------------------------------------

//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PORTAL_ADD_REQ )
	HOBJECT				handle;			
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PORTAL_REQ )
	HOBJECT				handle;			
	BYTE				byPoint;		// 0~7±îÁö ¼±ÅÃµÈ ÀÎµ¦½º
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PING )
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_WAR_FOG_UPDATE_REQ )
	HOBJECT				hObject;			// Object Handle		
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_GUILD_FUNCTION_ADD_REQ)
	HOBJECT				hGuildManagerNpc;
	BYTE				byFunction;			// Ãß°¡ÇÒ ±â´É ¹øÈ£ eDBO_GUILD_FUNCTION
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_GUILD_GIVE_ZENNY)
	HOBJECT				hGuildManagerNpc;
	DWORD				dwZenny;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_GUILD_BANK_START_REQ)
	HOBJECT				handle;			// Ã¢°í NpcHandle
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_GUILD_BANK_MOVE_REQ)
	HOBJECT				handle;			// Ã¢°í NpcHandle	
	BYTE				bySrcPlace;
	BYTE				bySrcPos;
	BYTE				byDestPlace;
	BYTE				byDestPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_GUILD_BANK_MOVE_STACK_REQ)
	HOBJECT				handle;			// Ã¢°í NpcHandle
	BYTE				bySrcPlace;
	BYTE				bySrcPos;
	BYTE				byDestPlace;
	BYTE				byDestPos;
	BYTE				byStackCount;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_GUILD_BANK_END_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_GUILD_BANK_ZENNY_REQ)
	HOBJECT				handle;			// NpcHandle
	DWORD				dwZenny;		// »©°Å³ª ³ÖÀ» ¾×¼ö
	bool				bIsSave;		// 1 Àº ³ÖÀ» °æ¿ì 0 Àº »©´Â°æ¿ì
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_CROSSFIRE_REQ )
	HOBJECT				hAnotherUser;		//À¯Á®°¡ ¼±ÅÃÇÑ User ÇÚµé
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_GUILD_CREATE_MARK_REQ )
	sDBO_GUILD_MARK		sMark;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_GUILD_CHANGE_MARK_REQ )
	sDBO_GUILD_MARK		sMark;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_BANK_LOAD_REQ )
	HOBJECT				handle;			// NpcHandle
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_GUILD_CHANGE_NAME_REQ )
	HOBJECT				hGuildManagerNpc;
	WCHAR				wszGuildName[NTL_MAX_SIZE_GUILD_NAME_IN_UNICODE + 1];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_PARTY_SHARETARGET_REQ )
	BYTE				bySlot;
	HOBJECT				hTarget;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_RIDE_ON_BUS_REQ )
	HOBJECT				hTarget;			// NpcHandle
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_RIDE_OFF_BUS_REQ )
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_TRANSFORM_CANCEL_REQ )
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SHOP_ITEM_IDENTIFY_REQ)
	HOBJECT				hNpchandle;		// NpcHandle
	BYTE				byPlace;
	BYTE				byPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_DICE_ROLL_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_JOIN_INDIVIDUAL_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_LEAVE_INDIVIDUAL_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_JOIN_TEAM_INFO_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_JOIN_TEAM_REQ)
	WCHAR				wszTeamName[NTL_MAX_LENGTH_BUDOKAI_TEAM_NAME_IN_UNICODE + 1];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_LEAVE_TEAM_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_LEAVE_TEAM_MEMBER_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_JOIN_INFO_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_JOIN_STATE_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_MUDOSA_INFO_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_MUDOSA_TELEPORT_REQ)
	BYTE			byMudosaIndex;				// ¹«µµ»ç ÀÎµ¦½º
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_PARTY_MAKER_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_SOCIAL_ACTION)
	ANIMATIONID			socialAction;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_PRIZEWINNER_NAME_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_PRIZEWINNER_TEAM_NAME_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_GM_BUDOKAI_SERVER_ENTER_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_GM_BUDOKAI_SERVER_LEAVE_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_GM_MATCH_PROGRESS_INFO_REQ)
	BYTE				byMatchType;	// eBUDOKAI_MATCH_TYPE
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_GM_MATCH_ARENA_ENTER_REQ)
	BYTE				byMatchType;		// eBUDOKAI_MATCH_TYPE
	BYTE				byMatchWorldType;	// eMATCHWORLD_TYPE
	BYTE				byMatchDepth;		// eBUDOKAI_MATCH_DEPTH
	BYTE				byMatchIndex;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_BUDOKAI_GM_MATCH_ARENA_LEAVE_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SCOUTER_TURN_ON)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SCOUTER_TURN_OFF)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SCOUTER_ITEM_SELL_REQ)
	BYTE			byPlace;
	BYTE			byPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SHOP_EVENTITEM_START_REQ)
	HOBJECT				handle;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SHOP_EVENTITEM_BUY_REQ)
	HOBJECT				handle;
	BYTE				byBuyCount;
	sSHOP_BUY_CART		sBuyData[NTL_MAX_BUY_SHOPPING_CART]; 
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SHOP_EVENTITEM_END_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SHOP_GAMBLE_BUY_REQ)
	HOBJECT				handle;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PARTY_DIFF_CHANGE_REQ)
	ePARTY_DUNGEON_STATE eDiff;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PARTY_DUNGEON_INIT_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SKILL_INIT_REQ)
	HOBJECT				handle;
END_PROTOCOL()
//------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_CHARM_DICE_ROLL_REQ)
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_INVEN_LOCK_REQ)
//	bool				bIsOpen;	// 1: Open 0:Close
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_INVEN_RESET_REQ)
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_INVEN_ACCEPTED_REQ)
//	BYTE				byItemSlot;	// ÆÄÆ¼ÀÎº¥ ÇØ´ç ½½·Ô ¾ÆÀÌÅÛ ³«Âû
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_INVEN_GIVE_REQ)
//	BYTE				byItemSlot;		// ÆÄÆ¼ÀÎº¥ ÇØ´ç ½½·Ô ¾ÆÀÌÅÛ 
//	HOBJECT				hTarget;
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_INVEN_DICE_START_REQ)
//	BYTE				byItemSlot;		// ÆÄÆ¼ÀÎº¥ ÇØ´ç ½½·Ô ¾ÆÀÌÅÛ	
//END_PROTOCOL()
////------------------------------------------------------------------
//BEGIN_PROTOCOL(UG_PARTY_INVEN_DICE_ROLL_REQ)
//END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_TELEPORT_CONFIRM_REQ)
	bool				bTeleport;		// true : È®ÀÎ, false : Ãë¼Ò
	BYTE				byTeleportIndex;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_HOIPOIMIX_ITEM_CHECK_REQ)			// ¸¸µé¾ÆÀÌÅÛ ¼±ÅÃ (ÇÊ¿äÇÑ ¾ÆÀÌÅÛ ¼ö·® Ã¼Å©)
	HOBJECT				objHandle;
	TBLIDX				recipeTblidx;	
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_HOIPOIMIX_ITEM_MAKE_REQ)			// ¾ÆÀÌÅÛ ¸¸µé±â
	HOBJECT				objHandle;
	TBLIDX				recipeTblidx;	
	bool				bIsEPUseSuccess;			// EP »ç¿ëÀ¯¹«
END_PROTOCOL()
//------------------------------------------------------------------			
BEGIN_PROTOCOL(UG_HOIPOIMIX_ITEM_MAKE_EP_REQ)		// ¾ÆÀÌÅÛ ¸¸µé±â Áß EP ³Ö±â
	TBLIDX				recipeTblidx;	
END_PROTOCOL()
//------------------------------------------------------------------	
BEGIN_PROTOCOL(UG_HOIPOIMIX_ITEM_MACHINE_DEL_REQ)			// ¸Ó½Å »èÁ¦
	HOBJECT				objHandle;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_HOIPOIMIX_JOB_SET_REQ)			// Àâ ¼¼ÆÃ
	HOBJECT				hNpchandle;
	BYTE				byRecipeType;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_HOIPOIMIX_JOB_RESET_REQ)			// Àâ ¸®¼Â
	HOBJECT				hNpchandle;
	BYTE				byRecipeType;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_VEHICLE_DIRECT_PLAY_CANCEL_NFY )
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_VEHICLE_ENGINE_START_REQ )
	TBLIDX				idFuelItemTblidx;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_VEHICLE_ENGINE_STOP_REQ )
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_VEHICLE_END_REQ )
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_VEHICLE_STUNT_NFY )
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_VEHICLE_FUEL_REMOVE_NFY )
	TBLIDX				idFuelItemTblidx;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_DOJO_CREATE_REQ )
	HOBJECT				hObject;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_DOJO_DELETE_REQ )
	HOBJECT				hObject;
	TBLIDX				dojoTblidx;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_DOJO_FUNCTION_ADD_REQ)
	HOBJECT				hGuildManagerNpc;
	BYTE				byFunction;			// Ãß°¡ÇÒ ±â´É ¹øÈ£ eDBO_GUILD_FUNCTION
	BYTE				byPlace;			// È£ÀÌÆ÷ÀÌ¶ôÀÌ ÇÊ¿ä½Ã ÀÎº¥À§Ä¡ ºÒÇÊ¿ä½Ã INVALID_BYTE
	BYTE				byPos;				// È£ÀÌÆ÷ÀÌ¶ôÀÌ ÇÊ¿ä½Ã ÀÎº¥À§Ä¡ ºÒÇÊ¿ä½Ã INVALID_BYTE
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_DOJO_BANK_HISTORY_REQ )
	HOBJECT				hObject;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_DOGI_CREATE_REQ )
	HOBJECT				hObject;
	sDBO_DOGI_DATA		sData;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_DOGI_CHANGE_REQ )
	HOBJECT				hObject;
	sDBO_DOGI_DATA		sData;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_GUILD_DOGI_CREATE_REQ )
	HOBJECT				hObject;
	sDBO_DOGI_DATA		sData;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_GUILD_DOGI_CHANGE_REQ )
	HOBJECT				hObject;
	sDBO_DOGI_DATA		sData;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_DOJO_SCRAMBLE_REQ )			// µµÀåÀïÅ»Àü ½ÅÃ» (µµÀüÀÚ)
	HOBJECT				hNpcHandle;				// µµÀå°ü¸®ÀÎ
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL( UG_DOJO_SCRAMBLE_RESPONSE_REQ )		// µµÀåÀïÅ»Àü ½ÅÃ»¿¡ ´ëÇÑ ÀÀ´ä (µµÀåÁÖÀÎ)
	bool				bIsAccept;				// ½ÅÃ»¿¡ ´ëÇÑ ÀÀ´ä 1:¹Þ±â 0:°ÅÀý
END_PROTOCOL()
//------------------------------------------------------------------						
BEGIN_PROTOCOL(UG_SHOP_NETPYITEM_START_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SHOP_NETPYITEM_BUY_REQ)
	BYTE				byBuyCount;
	sSHOP_BUY_CART		sBuyData[NTL_MAX_BUY_SHOPPING_CART]; 
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SHOP_NETPYITEM_END_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_WORLD_MAP_STATUS)
	bool				bIsWorldMapOpen;
	bool				bIsWorldMapOpen1;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_DOJO_NPC_INFO_REQ)
	HOBJECT				hNpcHandle;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_GAMEGUARD_AUTH_ANSWER_RES)
	BYTE				byDataSize;
	BYTE				abyData[UCHAR_MAX];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_GAMEGUARD_HACK_INFO_NFY)
	WORD				wDataSize;
	BYTE				abyData[USHRT_MAX];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_SKILL_CASTING_CANCELED_NFY)
	WORD				wResultCode;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PET_ATTACK_TARGET_NFY)
	BYTE				byAvatarType;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_DURATION_ITEM_BUY_REQ)
	TBLIDX				merchantTblidx;
	BYTE				byPos;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_DURATION_RENEW_REQ)
	HOBJECT				hItemHandle;
END_PROTOCOL()
//------------------------------------------------------------------

BEGIN_PROTOCOL(UG_GMT_UPDATE_REQ)
	sGAME_MANIA_TIME	sNext;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_LOC_AFTER_KNOCKDOWN_NFY)
	sVECTOR3			vCurLoc;
	sVECTOR2			vCurDir;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_LOC_AFTER_SLIDING_NFY)
	sVECTOR3			vCurLoc;
	sVECTOR2			vCurDir;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CHAR_LOC_AFTER_PUSH_NFY)
	sVECTOR3			vCurLoc;
	sVECTOR2			vCurDir;
END_PROTOCOL()

//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_PORTAL_START_REQ)
HOBJECT				handle;			// ÇØ´ç NPC °Ë»çÈÄ Æ÷Å» Á¤º¸ ·Îµù
END_PROTOCOL()

BEGIN_PROTOCOL(UG_CASHITEM_START_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CASHITEM_END_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CASHITEM_MOVE_REQ)
DWORD				dwProductId;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CASHITEM_USE_REQ)
DWORD				dwProductId;
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CASHITEM_HLSHOP_START_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CASHITEM_HLSHOP_END_REQ)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(UG_CASHITEM_BUY_REQ)
BYTE				byBuyCount;
DWORD				adwIdxHlsTable[NTL_MAX_BUY_HLSSHOP_CART];
END_PROTOCOL()
//------------------------------------------------------------------

/*
,
UG_CHAR_AIR_JUMP,
UG_CHAR_AIR_DASH,
UG_CHAR_AIR_FALLING,
UG_CHAR_AIR_END,
UG_CHAR_AIR_FOLLOW_MOVE,
UG_CHAR_AIR_ACCEL,
UG_CHAR_AIR_POSE_REQ,
*/
#pragma pack()





