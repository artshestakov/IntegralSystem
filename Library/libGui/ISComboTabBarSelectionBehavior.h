#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISComboTabBarSelectionBehavior : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTabBarSelectionBehavior(QWidget *parent);
	virtual ~ISComboTabBarSelectionBehavior();
};
//-----------------------------------------------------------------------------
