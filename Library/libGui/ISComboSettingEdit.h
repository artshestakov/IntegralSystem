#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISComboSettingEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboSettingEdit(QWidget *parent);
	virtual ~ISComboSettingEdit();
};
//-----------------------------------------------------------------------------
