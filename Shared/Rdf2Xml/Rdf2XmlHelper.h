#pragma once

#include <Winsock2.h>
#include <Windows.h>
#include "TableContainer.h"
#include "..\..\server\gslib\msxml2.h"
#import "..\..\debug\msxml4.dll"

class CRdf2XmlHelper
{
public:
	CRdf2XmlHelper();
	~CRdf2XmlHelper();

	bool CreateTableContainer(const char* pszPath, int byLoadMethod = CTable::LOADING_METHOD_BINARY);		
	bool SaveToXml_MobTable(const char* pszPath);
	bool SaveToXml_ItemTable(const char* pszPath);
	bool SaveToXml_SkillTable(const char* pszPath);
	bool SaveToXml_TextAllTable(const char* pszPath);
	bool SaveToXml_FormulaTable(const char* pszPath);
	bool SaveToXml_PcTable(const char* pszPath);
	bool SaveToXml_ExpTable(const char* pszPath);
	bool SaveToXml_QuestTextTable(const char* pszPath);
	bool SaveToXml_ObjectTable(const char* pszPath, int worldID);
	bool SaveToXml_PortalTable(const char* pszPath);
	bool SaveToXml_WorldTable(const char* pszPath);
	bool SaveToXml_MobSpawnTable(const char* pszPath, int worldID);
	bool SaveToXml_NPCSpawnTable(const char* pszPath, int worldID);
	bool SaveToXml_LandMarkTable(const char* pszPath);
	bool SaveToXml_DynamicObjectTable(const char* pszPath);
	bool SaveToXml_WorldMapTable(const char* pszPath);
	bool SaveToXml_WorldPlayTable(const char* pszPath);
	bool SaveToXml_WorldZoneTable(const char* pszPath);
	bool SaveToXml_VehicleTable(const char* pszPath);
	bool SaveToXml_MobMovePatternTable(const char* pszPath);
	bool SaveToXml_DirectionLinkTable(const char* pszPath);
	bool SaveToXml_ScriptLinkTable(const char* pszPath);
	bool SaveToXml_QuestNarrationTable(const char* pszPath);
	bool SaveToXml_BasicDropTable(const char* pszPath);
	bool SaveToXml_EachDropTable(const char* pszPath);
	bool SaveToXml_TypeDropTable(const char* pszPath);
	bool SaveToXml_NormalDropTable(const char* pszPath);
	bool SaveToXml_SuperiorDropTable(const char* pszPath);
	bool SaveToXml_ExcellentDropTable(const char* pszPath);
	bool SaveToXml_LegendaryDropTable(const char* pszPath);
	bool SaveToXml_ItemOptionTable(const char* pszPath);
	bool SaveToXml_SystemEffectTable(const char* pszPath);



	bool SaveToBinary_MobTable(const char* pszPath);
	bool SaveToBinary_ItemTable(const char* pszPath);
	bool SaveToBinary_SkillTable(const char* pszPath);
	bool SaveToBinary_TextAllTable(const char* pszPath);
	bool SaveToBinary_FormulaTable(const char* pszPath);
	bool SaveToBinary_PcTable(const char* pszPath);
	bool SaveToBinary_ExpTable(const char* pszPath);
	bool SaveToBinary_QuestTextTable(const char* pszPath);
	bool SaveToBinary_ObjectTable(const char* pszPath);
	bool SaveToBinary_PortalTable(const char* pszPath);
	
protected:
	CTableContainer* m_pTableContainer;	

};
