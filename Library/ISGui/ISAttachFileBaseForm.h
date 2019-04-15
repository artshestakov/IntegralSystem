#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISAttachFileBaseForm : public ISInterfaceDialogForm
{
	Q_OBJECT

signals:
	void UpdateList();

public:
	ISAttachFileBaseForm(QWidget *parent = 0);
	virtual ~ISAttachFileBaseForm();
};
//-----------------------------------------------------------------------------
