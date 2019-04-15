#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISDeviceComObject.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISDevice_Atol1101 : public ISDeviceComObject
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDevice_Atol1101(int device_user_id, QObject *parent = 0);
	virtual ~ISDevice_Atol1101();
};
//-----------------------------------------------------------------------------
