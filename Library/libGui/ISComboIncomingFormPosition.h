#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class ISComboIncomingFormPosition : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboIncomingFormPosition(QWidget *parent);
	virtual ~ISComboIncomingFormPosition();
};
//-----------------------------------------------------------------------------
