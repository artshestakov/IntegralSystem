#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISCenterSevenSelectBranchForm : public ISInterfaceDialogForm
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
