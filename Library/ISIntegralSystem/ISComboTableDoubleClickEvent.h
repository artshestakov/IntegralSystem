#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISComboTableDoubleClickEvent : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableDoubleClickEvent(QWidget *parent);
	virtual ~ISComboTableDoubleClickEvent();
};
//-----------------------------------------------------------------------------
