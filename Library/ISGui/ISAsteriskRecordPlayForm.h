#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISInterfaceDialogForm.h"
#include "ISPlayerWidget.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISAsteriskRecordPlayForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISAsteriskRecordPlayForm(const QString &FilePath, QWidget *parent = 0);
	virtual ~ISAsteriskRecordPlayForm();

private:
	ISPlayerWidget *PlayerWidget;
};
//-----------------------------------------------------------------------------
