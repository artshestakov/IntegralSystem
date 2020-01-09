#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISComboCalendarNoteType : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboCalendarNoteType(QWidget *parent);
	virtual ~ISComboCalendarNoteType();
};
//-----------------------------------------------------------------------------
