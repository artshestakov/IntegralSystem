#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISCenterSevenSelectBranchForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISCenterSevenSelectBranchForm(QWidget *parent = 0);
	virtual ~ISCenterSevenSelectBranchForm();

	int GetSelectedBranchID() const;

protected:
	void Select();

private:
	int SelectedBranchID;
};
//-----------------------------------------------------------------------------
