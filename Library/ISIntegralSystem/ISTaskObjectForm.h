#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISObjectFormBase.h"
#include "ISCheckEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTaskObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTaskObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
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
