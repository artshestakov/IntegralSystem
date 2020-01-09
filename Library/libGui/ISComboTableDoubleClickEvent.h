#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISComboTableDoubleClickEvent : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableDoubleClickEvent(QWidget *parent);
	virtual ~ISComboTableDoubleClickEvent();
};
//-----------------------------------------------------------------------------
