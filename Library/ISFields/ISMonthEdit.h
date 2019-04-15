#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISMonthEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISMonthEdit(QWidget *parent);
	virtual ~ISMonthEdit();

	void SelectCurrentMonth();
};
//-----------------------------------------------------------------------------
