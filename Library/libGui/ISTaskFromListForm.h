#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTaskBaseListForm.h"
//-----------------------------------------------------------------------------
class ISTaskFromListForm : public ISTaskBaseListForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskFromListForm(QWidget *parent = 0);
	virtual ~ISTaskFromListForm();
};
//-----------------------------------------------------------------------------
