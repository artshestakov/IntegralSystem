#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISIntegerEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISYearEdit : public ISIntegerEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISYearEdit(QWidget *parent);
	virtual ~ISYearEdit();

	void SelectCurrentYear();
};
//-----------------------------------------------------------------------------
