#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
#include "ISUserGroupWidget.h"
//-----------------------------------------------------------------------------
class ISUserGroupListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUserGroupListForm(QWidget *parent = 0);
	virtual ~ISUserGroupListForm();

protected:
	void SelectedGroup();

private:
	QLabel *Label;
	ISUserGroupWidget *UserGroupWidget;
};
//-----------------------------------------------------------------------------
