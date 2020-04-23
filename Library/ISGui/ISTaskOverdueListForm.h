#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTaskBaseListForm.h"
//-----------------------------------------------------------------------------
class ISTaskOverdueListForm : public ISTaskBaseListForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskOverdueListForm(QWidget *parent = 0);
	virtual ~ISTaskOverdueListForm();
};
//-----------------------------------------------------------------------------
