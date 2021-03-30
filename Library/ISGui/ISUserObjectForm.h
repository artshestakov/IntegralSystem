#pragma once
#ifndef _ISUSEROBJECTFORM_H_INCLUDED
#define _ISUSEROBJECTFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class ISUserObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUserObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISUserObjectForm();
	
protected:
	bool Save() override;

private:
	void PasswordManagement();
	void PasswordReset();
	void AccountLifeTimeChanged();

private:
	ISFieldEditBase *EditAccountLifeTime;
	ISFieldEditBase *EditAccountLifeTimeStart;
	ISFieldEditBase *EditAccountLifeTimeEnd;
};
//-----------------------------------------------------------------------------
#endif
