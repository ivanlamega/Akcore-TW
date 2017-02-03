#include "stdafx.h"
#include "NtlSfx.h"
#include "NtlPacketUA.h"
#include "NtlPacketAU.h"
#include "ResultCode.h"

#include "AuthServer.h"



//--------------------------------------------------------------------------------------//
//		Get the account ID and log in to Char Server									//
//--------------------------------------------------------------------------------------//
void CClientSession::SendCharLogInReq(CNtlPacket * pPacket, CAuthServer * app) 
{
	NTL_PRINT(PRINT_APP, "Client Received Login Request");

	sUA_LOGIN_REQ * req = (sUA_LOGIN_REQ *)pPacket->GetPacketData();

	CNtlPacket packet(sizeof(sAU_LOGIN_RES));
	sAU_LOGIN_RES* res = (sAU_LOGIN_RES *)packet.GetPacketData();


	res->wOpCode = AU_LOGIN_RES;
	memcpy(res->awchUserId, req->awchUserId, NTL_MAX_SIZE_USERID_UNICODE);
	strcpy_s((char*)res->abyAuthKey, NTL_MAX_SIZE_AUTH_KEY, "Dbo");

	app->db->prepare("CALL AuthLogin (? ,?, @acc_id, @result_code)");
	app->db->setString(1, Ntl_WC2MB(req->awchUserId));
	app->db->setString(2, Ntl_WC2MB(req->awchPasswd));
	app->db->execute();
	app->db->execute("SELECT @acc_id, @result_code");
	app->db->fetch(); 

	res->wResultCode = app->db->getInt("@result_code");
	res->accountId = app->db->getInt("@acc_id");
	res->serverID = 0;//Just a thing Server ID can be Zero xD
	string isEnabled = app->GetConfigFileEnabledMultipleServers();
	if (isEnabled == "true")
	{
		//Loading Max Server Files Configuration
		for(int i =0;i<app->GetConfigFileMaxServers();i++)
		{
			const char* CharIP = app->ServersConfig[i][0].c_str();
			const DWORD CharPort = atoi(app->ServersConfig[i][1].c_str());
			strcpy_s(res->aServerInfo[i].szCharacterServerIP, NTL_MAX_LENGTH_OF_IP, CharIP);
			res->aServerInfo[i].wCharacterServerPortForClient = CharPort;
			res->aServerInfo[i].dwLoad = 0;		
		}
		res->byServerInfoCount = app->GetConfigFileMaxServers();
		res->lastServerFarmId = INVALID_SERVERFARMID;
		res->dwAllowedFunctionForDeveloper = 0x0000ffff;
	}
	else
	{
		res->byServerInfoCount = 1;
		strcpy_s(res->aServerInfo[0].szCharacterServerIP, NTL_MAX_LENGTH_OF_IP, app->GetConfigFileExternalIP());
		res->dwAllowedFunctionForDeveloper = 0x0000ffff;
		res->aServerInfo[0].wCharacterServerPortForClient = 20300;
		res->aServerInfo[0].dwLoad = 0;
		res->lastServerFarmId = 0;
	}
	
	unsigned char buf[] = { 0xED, 0x03, 0x7f, 0xef, 0x03};
	CNtlPacket packet2(0x05);
	packet2.SetPacket(buf, 0x05);
	packet2.GetPacketHeader()->bEncrypt = false;
	PushHandshakePacket(&packet2);
	//printf("res->abyAuthKey= %d \n res->accountId=%d\n res->aServerInfo=%d\n res->awchUserId=%d\n res->byServerInfoCount=%d\n res->dwAllowedFunctionForDeveloper=%d\n res->lastServerFarmId=%d\n res->serverID=%d\n", res->abyAuthKey, res->accountId, res->aServerInfo, res->awchUserId, res->byServerInfoCount, res->dwAllowedFunctionForDeveloper, res->lastServerFarmId, res->serverID);

	packet.SetPacketLen(sizeof(sAU_LOGIN_RES));
	int rc = g_pApp->Send(this->GetHandle(), &packet);
		if (NTL_SUCCESS != rc)
		{
			NTL_PRINT(PRINT_APP, "Failed to send packet %d(%s)", rc, NtlGetErrorMessage(rc));
		}
		else
		{
			NTL_PRINT(PRINT_APP, "User %S send to charserver", req->awchUserId);
		}
}

//--------------------------------------------------------------------------------------//
//		Disconnect from Auth Server
//--------------------------------------------------------------------------------------//
void CClientSession::SendLoginDcReq(CNtlPacket * pPacket) 
{
	sUA_LOGIN_DISCONNECT_REQ * req = (sUA_LOGIN_DISCONNECT_REQ *)pPacket->GetPacketData();


	if (req->bIsEnteringCharacterServer == true)
	{
		CNtlPacket packet(sizeof(sAU_LOGIN_DISCONNECT_RES));
		sAU_LOGIN_DISCONNECT_RES * res = (sAU_LOGIN_DISCONNECT_RES *)packet.GetPacketData();
		res->wOpCode = AU_LOGIN_DISCONNECT_RES;
		cout << "Handing off to char Server" << endl;
		packet.SetPacketLen(sizeof(sAU_LOGIN_DISCONNECT_RES));
		int rc = g_pApp->Send(this->GetHandle(), &packet);
	}
	else
	{
		CNtlPacket packet(sizeof(sAU_LOGIN_DISCONNECT_RES));
		sAU_LOGIN_DISCONNECT_RES * res = (sAU_LOGIN_DISCONNECT_RES *)packet.GetPacketData();
		res->wOpCode = AU_LOGIN_DISCONNECT_RES;
		packet.SetPacketLen(sizeof(sAU_LOGIN_DISCONNECT_RES));
		int rc = g_pApp->Send(this->GetHandle(), &packet);
		cout << "Quiting Auth Server" << endl;
		this->Disconnect(true);
	}
}
