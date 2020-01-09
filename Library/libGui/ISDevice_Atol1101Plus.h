#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISDeviceComObject.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDevice_Atol1101Plus : public ISDeviceComObject
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDevice_Atol1101Plus(int device_user_id, QObject *parent = 0);
	virtual ~ISDevice_Atol1101Plus();
};
//-----------------------------------------------------------------------------
