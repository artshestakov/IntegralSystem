#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISUserListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUserListForm(QWidget *parent = 0);
	virtual ~ISUserListForm();

protected:
	void CreateCopy() override;
	void Edit() override;

private:
	void PasswordManagement();
	void PasswordReset();
	bool CheckThisUser(); //ѕроверка редактировани€ текущего пользовател€
};
//-----------------------------------------------------------------------------
