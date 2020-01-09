#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISPlayerWidget.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISAsteriskRecordPlayForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISAsteriskRecordPlayForm(const QString &FilePath, QWidget *parent = 0);
	virtual ~ISAsteriskRecordPlayForm();

private:
	ISPlayerWidget *PlayerWidget;
};
//-----------------------------------------------------------------------------
