#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISTaskListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskListForm(QWidget *parent = 0);
	virtual ~ISTaskListForm();

	void Edit() override;
};
//-----------------------------------------------------------------------------
