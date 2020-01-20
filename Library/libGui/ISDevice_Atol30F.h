#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISDeviceComObject.h"
//-----------------------------------------------------------------------------
class ISDevice_Atol30F : public ISDeviceComObject
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDevice_Atol30F(int device_user_id, QObject *parent = 0);
	virtual ~ISDevice_Atol30F();
};
//-----------------------------------------------------------------------------
