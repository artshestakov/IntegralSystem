#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISWeekDayEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISWeekDayEdit(QWidget *parent);
	virtual ~ISWeekDayEdit();
};
//-----------------------------------------------------------------------------
