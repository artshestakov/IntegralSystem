#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDeviceSubSystem : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDeviceSubSystem(QWidget *parent = 0);
	virtual ~ISDeviceSubSystem();
};
//-----------------------------------------------------------------------------
