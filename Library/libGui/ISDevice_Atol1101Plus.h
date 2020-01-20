#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISDeviceComObject.h"
//-----------------------------------------------------------------------------
class ISDevice_Atol1101Plus : public ISDeviceComObject
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDevice_Atol1101Plus(int device_user_id, QObject *parent = 0);
	virtual ~ISDevice_Atol1101Plus();
};
//-----------------------------------------------------------------------------
