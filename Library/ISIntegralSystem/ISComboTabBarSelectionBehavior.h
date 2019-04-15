#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISComboTabBarSelectionBehavior : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTabBarSelectionBehavior(QWidget *parent);
	virtual ~ISComboTabBarSelectionBehavior();
};
//-----------------------------------------------------------------------------
