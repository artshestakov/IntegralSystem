#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISDeviceComObject.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISDevice_Atol30F : public ISDeviceComObject
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDevice_Atol30F(int device_user_id, QObject *parent = 0);
	virtual ~ISDevice_Atol30F();
};
//-----------------------------------------------------------------------------
