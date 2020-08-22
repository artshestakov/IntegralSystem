#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class ISHistoryForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISHistoryForm(QWidget *parent = 0);
	virtual ~ISHistoryForm();

private:
	void Open(QListWidgetItem *ListWidgetItem);
};
//-----------------------------------------------------------------------------
