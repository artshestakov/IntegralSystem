#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISMonthEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISMonthEdit(QWidget *parent);
	virtual ~ISMonthEdit();

	void SelectCurrentMonth();
};
//-----------------------------------------------------------------------------
