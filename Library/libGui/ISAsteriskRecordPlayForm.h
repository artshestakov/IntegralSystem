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
	ISAsteriskRecordPlayForm(const QString &FilePath, QWidget *parent = 0);
	virtual ~ISAsteriskRecordPlayForm();

private:
	ISPlayerWidget *PlayerWidget;
};
//-----------------------------------------------------------------------------
