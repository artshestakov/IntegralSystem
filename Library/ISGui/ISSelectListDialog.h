#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISSelectListForm.h"
//-----------------------------------------------------------------------------
class ISSelectListDialog : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISSelectListDialog(ISNamespace::SelectListMode SelectMode, const QString &TableName, int SelectObjectID);
	virtual ~ISSelectListDialog();

	bool Exec() override;
	ISObjectPair GetSelectedObject() const;
	ISVectorUInt GetSelectedObjects() const;

protected:
	void AfterShowEvent() override;

private:
	void Selected();

private:
	ISSelectListForm *SelectListForm;
};
//-----------------------------------------------------------------------------
