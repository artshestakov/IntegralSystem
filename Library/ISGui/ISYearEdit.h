#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISIntegerEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISYearEdit : public ISIntegerEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISYearEdit(QWidget *parent);
	virtual ~ISYearEdit();

	void SelectCurrentYear();
};
//-----------------------------------------------------------------------------
