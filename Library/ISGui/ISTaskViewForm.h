#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class ISTaskViewForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISTaskViewForm(int task_id, QWidget *parent = 0);
	virtual ~ISTaskViewForm();

private:
	QHBoxLayout *LayoutHorizontal;
	QVBoxLayout *LayoutLeft;
	QVBoxLayout *LayoutRight;

private:
	int TaskID;
};
//-----------------------------------------------------------------------------
