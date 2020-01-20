#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class ISComboTableDateFormat : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableDateFormat(QWidget *parent);
	virtual ~ISComboTableDateFormat();
};
//-----------------------------------------------------------------------------
