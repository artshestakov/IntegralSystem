#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISUserObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUserObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISUserObjectForm();
	
protected:
	void AfterShowEvent() override;
	bool Save() override;
	void SavedEvent() override;
	void PasswordChange();
	void AccountLifeTimeChanged();

private:
	ISFieldEditBase *EditLogin;
	ISFieldEditBase *EditAccountLifeTime;
	ISFieldEditBase *EditAccountLifeTimeStart;
	ISFieldEditBase *EditAccountLifeTimeEnd;

	QString LoginOld;
};
//-----------------------------------------------------------------------------
