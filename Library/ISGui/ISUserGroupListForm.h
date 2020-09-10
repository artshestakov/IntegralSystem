#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISUserGroupListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUserGroupListForm(QWidget *parent = 0);
	virtual ~ISUserGroupListForm();

private:
	void SelectedGroup();
};
//-----------------------------------------------------------------------------
