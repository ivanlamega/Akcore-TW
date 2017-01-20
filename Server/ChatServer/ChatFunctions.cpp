#include "stdafx.h"
#include "ChatServer.h"
void ChatFunctionsClass::AddRemoveFriend(int CharID, int TargetID, bool bRemove,bool toBlackList)
{
	CChatServer * app = (CChatServer*)NtlSfxGetApp();
	if (bRemove)
	{
		app->db->prepare("DELETE FROM buddylist WHERE owner_id = ? AND friend_id = ?");
		app->db->setInt(1, CharID);
		app->db->setInt(2, TargetID);
		app->db->execute();
	}
	else
	{
		if (toBlackList)
		{
			app->db->prepare("UPDATE buddylist SET moveBlackList = 1 WHERE owner_id = ? AND friend_id = ?");
			app->db->setInt(1, CharID);
			app->db->setInt(2, TargetID);
			app->db->execute();
		}
		else
		{
			app->db->prepare("INSERT INTO buddylist (owner_id,friend_id) VALUES (?,?)");
			app->db->setInt(1, CharID);
			app->db->setInt(2, TargetID);
			app->db->execute();
		}		
	}
}

void ChatFunctionsClass::AddRemoveBlackList(int CharID, int TargetID, bool bRemove)
{
	CChatServer * app = (CChatServer*)NtlSfxGetApp();
	if (bRemove)
	{
		app->db->prepare("DELETE FROM blacklist WHERE owner_id = ? AND target_id = ?");
		app->db->setInt(1, CharID);
		app->db->setInt(2, TargetID);
		app->db->execute();

		app->db->prepare("DELETE FROM buddylist WHERE owner_id = ? AND friend_id = ?");
		app->db->setInt(1, CharID);
		app->db->setInt(2, TargetID);
		app->db->execute();
	}
	else
	{
		app->db->prepare("INSERT INTO blacklist (owner_id,target_id) VALUES (?,?)");
		app->db->setInt(1, CharID);
		app->db->setInt(2, TargetID);
		app->db->execute();
	}
}

//void ChatFunctionsClass::ErrorMessages()
//{
//
//}