#include "stdafx.h"
#include "Object.h"


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
const char * s_object_type_string[ MAX_OBJTYPE ] = 
{
	"OBJTYPE_PC",
	"OBJTYPE_NPC",
	"OBJTYPE_MOB",
	"OBJTYPE_SUMMON_PET",
	"OBJTYPE_ITEM_PET",
	"OBJTYPE_DROPMONEY",
	"OBJTYPE_DROPITEM",
	"OBJTYPE_TOBJECT",
	"OBJTYPE_DYNAMIC",
	"OBJTYPE_ITEM",
};


//-----------------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------------
const char * NtlGetObjectTypeString(eOBJTYPE eObjType)
{
	if( eObjType >= MAX_OBJTYPE )
	{
		return "UNDEFINED_OBJECT_TYPE";
	}

	return s_object_type_string[ eObjType ];
}