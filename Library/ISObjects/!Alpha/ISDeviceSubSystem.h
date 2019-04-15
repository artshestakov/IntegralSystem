#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISDeviceSubSystem : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDeviceSubSystem(QWidget *parent = 0);
	virtual ~ISDeviceSubSystem();
};
//-----------------------------------------------------------------------------
