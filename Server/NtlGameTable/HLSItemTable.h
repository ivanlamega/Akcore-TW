//***********************************************************************************
//
//	File		:	HLSItemTable.h
//
//	Begin		:	2009-8-14
//
//	Copyright	:	ㄏ NTL-Inc Co., Ltd
//
//	Author		:	Chung Doo sup   ( john@ntl-inc.com )
//
//	Desc		:	
//
//***********************************************************************************

#pragma once

#include "Table.h"
#include "NtlItem.h"

#include "NtlHlsItem.h"

const DWORD		DBO_MAX_LENGTH_HLSITEM_ID_TEXT = 128;
const DWORD		DBO_MAX_LENGTH_HLSITEM_NAME_TEXT = 20;

struct sSUB_ITEM
{
	TBLIDX	itemTblidx;
	BYTE	byStackCount;
};

#pragma pack(push, 4)
struct sHLS_ITEM_TBLDAT : public sTBLDAT
{
public:
	//WCHAR            wszName[20 + 1];
	//WCHAR            wszCJIProductID[128 + 1];
	////need Check all Struture 
	//WORD            wHLSItemType;
	//bool            bOnSale;
	//BYTE            bySellType;
	//WORD            unk2;
	//BYTE            byDiscount;    // ÇÒÀÎ·ü
	//BYTE            byStackCount;
	//bool            bCjVip;        //CJ VIP ÇÒÀÎ°¡´ÉÇÑ°¡
	//BYTE            byVipDiscount; // CJ VIP ÇÒÀÎ·ü
	//WORD            wDisplayBitFlag;        //eHLS_DISPLAY_ITEM_FUNC_BIT_FLAG
	//sSUB_ITEM       asSubItem[5];
	//sDBO_TIME       sLimitedStart;
	//sDBO_TIME       sLimitedEnd;
	//DWORD           unk6;
	/////// all before need to be checked
	////Correct IDS
	//DWORD           itemTblidx;//Next Item ID "asSubItem"
	//DWORD           itemTblidx2;//Next Item ID "asSubItem"
	//DWORD           itemTblidx3;//Next Item ID "asSubItem"
	///////////////////////
	//DWORD           unk7;
	////Maybe Price
	//DWORD           Price;//Price CP"
	//////////??????//////
	//BYTE           unk8;
	//BYTE           unk9;
	//BYTE           unk10;
	//BYTE           unk11;
	//DWORD           unk12;
	//DWORD           unk13;
	//DWORD           unk14;
	//DWORD           unk15;
	//DWORD           unk16;
	/////Need Check 32 Byte
	//BYTE        TEST1[32];

	WCHAR            wszName[20 + 1];
	WCHAR            wszCJIProductID[128 + 1];
	WCHAR            wszIcon[32 + 1];
	//need Check all Struture 
	WORD            wHLSItemType;
	bool            bOnSale;
	BYTE            bySellType;
	BYTE            byDiscount;    // ÇÒÀÎ·ü
	BYTE            byStackCount;
	bool            bCjVip;        //CJ VIP ÇÒÀÎ°¡´ÉÇÑ°¡
	BYTE            byVipDiscount; // CJ VIP ÇÒÀÎ·ü
	WORD            wDisplayBitFlag;        //eHLS_DISPLAY_ITEM_FUNC_BIT_FLAG
	BYTE				unk1;
	sSUB_ITEM       asSubItem[5];
	sDBO_TIME       sLimitedStart;
	sDBO_TIME       sLimitedEnd;
	///// all before need to be checked
	//Correct IDS
	DWORD           itemTblidx;//Next Item ID "asSubItem"
	DWORD           itemTblidx2;//Next Item ID "asSubItem"
	DWORD           itemTblidx3;//Next Item ID "asSubItem"
	/////////////////////
	//Maybe Price
	////////??????//////
	WORD 		Price;
public:

	virtual int GetDataSize()
	{
		return sizeof(*this) - sizeof(void*);
	}
};
#pragma pack(pop)

class CHLSItemTable : public CTable
{
public:

	CHLSItemTable(void);
	virtual ~CHLSItemTable(void);
	
	bool Create(DWORD dwCodePage);
	void Destroy();

protected:
	void Init();

public:
	sTBLDAT *			FindData(TBLIDX tblidx); 
	
protected:
	WCHAR** GetSheetListInWChar() { return &(CHLSItemTable::m_pwszSheetList[0]); }
	void* AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage);
	bool DeallocNewTable(void* pvTable, WCHAR* pwszSheetName);
	bool AddTable(void * pvTable, bool bReload);
	bool SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData);

public:
	virtual bool				LoadFromBinary(CNtlSerializer& serializer, bool bReload);
	virtual bool				SaveToBinary(CNtlSerializer& serializer);

private:
	static WCHAR* m_pwszSheetList[];
};