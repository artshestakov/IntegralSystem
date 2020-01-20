#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISIntegerEdit.h"
//-----------------------------------------------------------------------------
class ISYearEdit : public ISIntegerEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISYearEdit(QWidget *parent);
	virtual ~ISYearEdit();

	void SelectCurrentYear();
};
//-----------------------------------------------------------------------------
