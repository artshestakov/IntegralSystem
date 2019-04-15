#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISListBaseForm.h"
#include "ISProcessForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISUserListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUserListForm(QWidget *parent = 0);
	virtual ~ISUserListForm();

protected:
	void CreateCopy() override;
	void Edit() override;
	void LoadDataAfterEvent() override;
	void ChangePassword();
	void UpdateLabelOnline();

	bool CheckThisUser(); //ѕроверка редактировани€ текущего пользовател€

private:
	QLabel *LabelOnline;
};
//-----------------------------------------------------------------------------
