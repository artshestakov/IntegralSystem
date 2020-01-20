#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class ISComboTableDoubleClickEvent : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableDoubleClickEvent(QWidget *parent);
	virtual ~ISComboTableDoubleClickEvent();
};
//-----------------------------------------------------------------------------
