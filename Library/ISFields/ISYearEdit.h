#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISIntegerEdit.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISYearEdit : public ISIntegerEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISYearEdit(QWidget *parent);
	virtual ~ISYearEdit();

	void SelectCurrentYear();
};
//-----------------------------------------------------------------------------
