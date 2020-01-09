#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISWeekDayEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISWeekDayEdit(QWidget *parent);
	virtual ~ISWeekDayEdit();
};
//-----------------------------------------------------------------------------
