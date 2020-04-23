#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTaskBaseListForm.h"
//-----------------------------------------------------------------------------
class ISTaskTodayListForm : public ISTaskBaseListForm
{
	Q_OBJECT
		
public:
	Q_INVOKABLE ISTaskTodayListForm(QWidget *parent = 0);
	virtual ~ISTaskTodayListForm();
};
//-----------------------------------------------------------------------------
