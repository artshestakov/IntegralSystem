#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISAttachFileBaseForm : public ISInterfaceDialogForm
{
	Q_OBJECT

signals:
	void UpdateList();

public:
	ISAttachFileBaseForm(QWidget *parent = 0);
	virtual ~ISAttachFileBaseForm();
};
//-----------------------------------------------------------------------------
