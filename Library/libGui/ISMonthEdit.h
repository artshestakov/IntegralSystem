#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISMonthEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISMonthEdit(QWidget *parent);
	virtual ~ISMonthEdit();

	void SelectCurrentMonth();
};
//-----------------------------------------------------------------------------
