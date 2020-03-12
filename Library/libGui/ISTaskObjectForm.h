#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISObjectFormBase.h"
#include "ISCheckEdit.h"
//-----------------------------------------------------------------------------
class ISTaskObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskObjectForm(ISNamespace::ObjectFormType form_type, PMetaTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISTaskObjectForm();

protected:
	void AfterShowEvent() override;
	bool Save() override;

	void TakeToWork();

private:
	QAction *ActionTakeToWork;
	ISCheckEdit *CheckFavorite;
};
//-----------------------------------------------------------------------------
