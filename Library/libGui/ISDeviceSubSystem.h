#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISDeviceSubSystem : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDeviceSubSystem(QWidget *parent = 0);
	virtual ~ISDeviceSubSystem();
};
//-----------------------------------------------------------------------------
