#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISSelectListForm.h"
//-----------------------------------------------------------------------------
class ISSelectDialogForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISSelectDialogForm(ISNamespace::SelectListMode SelectMode, const QString &TableName, int SelectObjectID, QWidget *parent = 0);
	virtual ~ISSelectDialogForm();

	bool Exec() override;
	int GetSelectedObject() const;
	ISVectorInt GetSelectedObjects() const;

protected:
	void Selected();

private:
	ISSelectListForm *SelectListForm;
};
//-----------------------------------------------------------------------------
