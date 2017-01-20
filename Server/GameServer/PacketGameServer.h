#pragma once

#include "SharedType.h"
#include "AdmFunctions.h"
#include "GsFunctions.h"



RwUInt32 AcquireSerialId(void)
{
	if(m_uiSerialId++)
	{
		if(m_uiSerialId == 0xffffffff)//INVALID_SERIAL_ID)
			m_uiSerialId = 0;
	}

	return m_uiSerialId;
}

RwUInt32 AcquireItemSerialId(void)
{
	if (m_iSerialId++)
	{
		if (m_iSerialId == 0xffffffff)//INVALID_SERIAL_ID)
			m_iSerialId = 0;
	}

	return m_iSerialId;
}

