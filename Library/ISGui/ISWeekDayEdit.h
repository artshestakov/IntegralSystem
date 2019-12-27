#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISWeekDayEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISWeekDayEdit(QWidget *parent);
	virtual ~ISWeekDayEdit();
};
//-----------------------------------------------------------------------------
