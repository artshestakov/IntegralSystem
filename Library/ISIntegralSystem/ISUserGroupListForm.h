#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISListBaseForm.h"
#include "ISUserGroupWidget.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISUserGroupListForm : public ISListBaseForm
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
