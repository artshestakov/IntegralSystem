#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISObjectFormBase.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISHighwayOrganizationObjectForm : public ISObjectFormBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISHighwayOrganizationObjectForm(ISNamespace::ObjectFormType form_type, PMetaClassTable *meta_table, QWidget *parent, int object_id = 0);
	virtual ~ISHighwayOrganizationObjectForm();

protected:
	void AfterShowEvent() override;
	bool Save() override;
	void BranchClicked();
	void BindBranch();
	void RebindBranch();
	void ReloadBranch();

private:
	QAction *ActionBranchMenu;
	QAction *ActionRebind;
};
//-----------------------------------------------------------------------------
