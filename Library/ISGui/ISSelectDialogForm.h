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
	ISSelectDialogForm(ISNamespace::SelectListMode SelectMode, const QString &TableName, int SelectObjectID);
	virtual ~ISSelectDialogForm();

	bool Exec() override;
	unsigned int GetSelectedObject() const;
	ISVectorUInt GetSelectedObjects() const;

protected:
	void AfterShowEvent() override;

private:
	void Selected();

private:
	ISSelectListForm *SelectListForm;
};
//-----------------------------------------------------------------------------
