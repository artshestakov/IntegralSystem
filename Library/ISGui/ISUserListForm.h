#pragma once
//-----------------------------------------------------------------------------
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISUserListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUserListForm(QWidget *parent = 0);
	virtual ~ISUserListForm();
};
//-----------------------------------------------------------------------------
