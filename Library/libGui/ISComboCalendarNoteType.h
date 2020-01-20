#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class ISComboCalendarNoteType : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboCalendarNoteType(QWidget *parent);
	virtual ~ISComboCalendarNoteType();
};
//-----------------------------------------------------------------------------
