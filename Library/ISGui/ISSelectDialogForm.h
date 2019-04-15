#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISSelectListForm.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISSelectDialogForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISSelectDialogForm(ISNamespace::SelectListMode SelectMode, const QString &TableName, int SelectObjectID, QWidget *parent = 0);
	virtual ~ISSelectDialogForm();

	bool Exec() override;
	int GetSelectedObject() const;
	QVectorInt GetSelectedObjects() const;

protected:
	void Selected();

private:
	ISSelectListForm *SelectListForm;
};
//-----------------------------------------------------------------------------
