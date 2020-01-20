#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class ISComboTableSelectionBehavior : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableSelectionBehavior(QWidget *parent);
	virtual ~ISComboTableSelectionBehavior();
};
//-----------------------------------------------------------------------------
