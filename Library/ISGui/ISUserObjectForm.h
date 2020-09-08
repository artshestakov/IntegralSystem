#pragma once
#ifndef _ISUSEROBJECTFORM_H_INCLUDED
#define _ISUSEROBJECTFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISObjectFormBase.h"
#include "ISUserPhotoCreator.h"
//-----------------------------------------------------------------------------
class ISUserObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUserObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISUserObjectForm();
	
protected:
	void AfterShowEvent() override;
	bool Save() override;
	void SavedEvent() override;
	void SaveAfter() override;

private:
	void PasswordChange();
	void PasswordDelete();
	void FIOChanged();
	void AccountLifeTimeChanged();

private:
	ISFieldEditBase *EditLogin;
	ISFieldEditBase *EditAccountLifeTime;
	ISFieldEditBase *EditAccountLifeTimeStart;
	ISFieldEditBase *EditAccountLifeTimeEnd;

	QString CurrentLogin;
	ISUserPhotoCreator UserPhotoCreator;
};
//-----------------------------------------------------------------------------
#endif
