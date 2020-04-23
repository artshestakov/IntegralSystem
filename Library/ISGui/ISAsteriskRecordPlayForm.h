#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISPlayerWidget.h"
//-----------------------------------------------------------------------------
class ISAsteriskRecordPlayForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISAsteriskRecordPlayForm(const QString &FilePath);
	virtual ~ISAsteriskRecordPlayForm();

private:
	ISPlayerWidget *PlayerWidget;
};
//-----------------------------------------------------------------------------
