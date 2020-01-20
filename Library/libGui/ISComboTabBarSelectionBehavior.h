#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class ISComboTabBarSelectionBehavior : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTabBarSelectionBehavior(QWidget *parent);
	virtual ~ISComboTabBarSelectionBehavior();
};
//-----------------------------------------------------------------------------
