#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISComboCalendarNoteType : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboCalendarNoteType(QWidget *parent);
	virtual ~ISComboCalendarNoteType();
};
//-----------------------------------------------------------------------------
